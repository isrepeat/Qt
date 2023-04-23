//#include "pch.h"
#include "Direct2DRenderer.h"
#include "Helpers.h"
#include "System.h"
#include <d2d1effects.h>
#include <cassert>
#include <sstream>
#include <numbers>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite")

Direct2DRenderer::~Direct2DRenderer() {
	Stop();
}

void Direct2DRenderer::SetNeedTextureHandler(std::function<void()> handler) {
	needTextureHandler = handler;
}

void Direct2DRenderer::SetLostFrameRequestHandler(std::function<void(std::vector<uint8_t>, uint64_t)> handler) {
	lostFrameHandler = handler;
}

void Direct2DRenderer::SetNewFrameChunksCount(uint8_t totalChunks) {
	std::lock_guard lk{ mutex };
	chunks.resize(totalChunks);
}

void Direct2DRenderer::SetDecodeHandler(std::function<std::vector<uint8_t>(std::vector<uint8_t>&)> handler) {
	decodeHandler = handler;
}

void Direct2DRenderer::AddFrameChunk(Frame&& chunk, uint8_t index, uint8_t chunksCount) {
	std::lock_guard lk{ mutex };
	//controlChunk = chunk;

	chunks.insert(chunks.begin() + index, std::move(chunk));
	lastChunkReceiveTimestamp = H::GetCurrentTimestamp<std::chrono::milliseconds>();
	isWaitingChunks = false;
}

//TODO remove, can get data from swapchain
void Direct2DRenderer::SetFrameConfiguration(FrameConfigurationData frameConfiguration) {
	auto logMessage = "Frame configuration: width: " + std::to_string(frameConfiguration.size.width) + ", height: " + std::to_string(frameConfiguration.size.height);

	frameConfigurationData = frameConfiguration;
}

void Direct2DRenderer::SetHwnd(HWND hwnd) {
	std::lock_guard<std::mutex> lk(mutexDrawing); // guard start initialize and parallel resize [CHECK if need it]

	this->hwnd = hwnd;
	InitDirect2D();

	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(directWriteFactory.GetAddressOf())
	);
	H::System::ThrowIfFailed(hr);

	hr = directWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 42.0f, L"en-US", textFormat.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	hr = d2d1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), blackBrush.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	hr = d2d1DeviceContext->CreateEffect(CLSID_D2D1Scale, scaleEffect.GetAddressOf());
	H::System::ThrowIfFailed(hr);
	hr = scaleEffect->SetValue(D2D1_SCALE_PROP_SHARPNESS, 1.f);
	H::System::ThrowIfFailed(hr);
	hr = scaleEffect->SetValue(D2D1_SCALE_PROP_INTERPOLATION_MODE, D2D1_SCALE_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
	H::System::ThrowIfFailed(hr);
	hr = scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(scaleX, scaleY));
	H::System::ThrowIfFailed(hr);


	hr = d2d1DeviceContext->CreateEffect(CLSID_D2D12DAffineTransform, rotateEffect.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	InitDirectRenderTarget(frameConfigurationData.GetPixelFormatAsDxgiFormat());
}



void Direct2DRenderer::SetFps(uint8_t fps) {
	renderingDelay = 1000 / fps;
}


void Direct2DRenderer::Resizing(bool enable) {
	resizing = enable;
}


void Direct2DRenderer::SetWindowSize(Size newSize) {
	std::lock_guard<std::mutex> lock(mutexDrawing);

	if (swapChain != nullptr) {
		d2d1DeviceContext->SetTarget(nullptr);
		d2d1RenderTargetBitmap->Release();

		// We must release all refs on swapChain->GetBuffer(...) before ResizeBuffers(...)
		auto hr = swapChain->ResizeBuffers(0, newSize.width, newSize.height, DXGI_FORMAT_UNKNOWN, 0);
		H::System::ThrowIfFailed(hr);

		CreateBitmapFromSwapchain();
		d2d1DeviceContext->SetTarget(d2d1RenderTargetBitmap.Get());
	}

	auto configuration = frameConfigurationData;

	// Because we will rotate frame (on 90 ) swap height & width for calculate corrent scaleFactor
	if (configuration.desktopOrientation == DesktopOrientation::Portrait ||
		configuration.desktopOrientation == DesktopOrientation::PortraitFlipped) {
		std::swap(configuration.size.width, configuration.size.height);
	}

	// newSize - current size RenderWindow
	auto scaleX = static_cast<double>(newSize.width) / configuration.size.width;
	auto scaleY = static_cast<double>(newSize.height) / configuration.size.height;

	float scaleFactor = scaleX;
	if (configuration.size.height * scaleFactor > newSize.height)
		scaleFactor = scaleY; // somthing like limit for scaleX


	if (scaleEffect != nullptr) {
		auto hr = scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(scaleFactor, scaleFactor));
		H::System::ThrowIfFailed(hr);


		float PI = 3.1415f;
		float m11 = 1, m12 = 0;
		float m21 = 0, m22 = 1;
		float offsetX = 0, offsetY = 0;
		if (configuration.desktopOrientation == DesktopOrientation::Landscape) {
		}
		if (configuration.desktopOrientation == DesktopOrientation::LandscapeFlipped) {
			float rad = PI;
			m11 = std::cos(rad); m12 = -std::sin(rad);
			m21 = std::sin(rad); m22 = std::cos(rad);
			offsetX = configuration.size.width;
			offsetY = configuration.size.height;
		}
		if (configuration.desktopOrientation == DesktopOrientation::Portrait) {
			// Position formule below get from follow:
			// configuration.size.width*scaleX = edge position        
			//												  [limit scaleX]  [rest of scaleX]
			// 1. (configuration.size.width)*scaleX [scaleX  =  scaleFactor  +  restScale  =  scaleFactor + scaleX-scaleFactor]
			//                    [scaleFactor -->keep at left corner]
			// 2. (configuration.size.width)*scaleFactor + (configuration.size.width)*(scaleX - scaleFactor)
			//																[/2 -->	kepp at center]	
			// 3. (configuration.size.width)*scaleFactor + (configuration.size.width/2)*(scaleX - scaleFactor)

			float rad = -PI / 2;

			// TODO: replace on precomputed values
			m11 = std::cos(rad); m12 = -std::sin(rad);
			m21 = std::sin(rad); m22 = std::cos(rad);
			offsetX = configuration.size.width;
		}
		if (configuration.desktopOrientation == DesktopOrientation::PortraitFlipped) {
			float rad = +PI / 2;
			m11 = std::cos(rad); m12 = -std::sin(rad);
			m21 = std::sin(rad); m22 = std::cos(rad);
			offsetY = configuration.size.height;
		}

		float freeWidthFrame = (configuration.size.width) * (scaleX - scaleFactor); // rest of scale X
		float freeHeightFrame = (configuration.size.height) * (scaleY - scaleFactor); // rest of scale Y


		D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F(
			m11, m12,
			m21, m22,
			offsetX * scaleFactor + freeWidthFrame / 2,
			offsetY * scaleFactor + freeHeightFrame / 2
		);
		hr = rotateEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
		H::System::ThrowIfFailed(hr);
	}
}

//void Direct2DRenderer::Start() {
//	if (d2d1DeviceContext == nullptr)
//		throw std::exception("Direct2D components uninitialized");
//	////chunks = std::move(previousFrameChunks);
//
//	Frame frame;
//	frame.dirtyRegions.push_back(DirtyImageRegion{ 0,0, frameConfigurationData.size.width, frameConfigurationData.size.height });
//
//	chunks.clear();
//	chunks.push_back(std::move(frame));
//	controlChunk = chunks[0];
//
//	////lastData.resize(frameConfigurationData.size.width * frameConfigurationData.size.height * 4);
//	////for (int n = 0; n < frameConfigurationData.size.width * frameConfigurationData.size.height / 2; n++) {
//	////	int pixNum = n % frameConfigurationData.size.width;
//	////	if (100 < pixNum && pixNum < frameConfigurationData.size.width - 100) {
//	////		for (int k = 0; k < 4; k++) {
//	////			if (k == 1)
//	////				lastData[n * 4 + k] = 0xFF;
//	////		}
//	////	}
//	////}
//
//	working = true;
//	renderingThread = std::thread([this] {
//		//frameRequestHandler();
//		needTextureHandler();
//		timeTotal = H::GetCurrentTimestamp<std::chrono::milliseconds>();
//
//		// If often switch monitors maybe be crash without delay, check ...
//		//std::this_thread::sleep_for(std::chrono::milliseconds(100)); // wait until d2d1DeviceContext is fully init (when monitor was changed)
//		while (working) {
//			RenderingRoutine();
//		}
//		});
//}

void Direct2DRenderer::Start() {
	assert(d2d1DeviceContext);
	if (!d2d1DeviceContext) {
		throw std::exception("Direct2D components uninitialized");
	}

	chunks = std::move(previousFrameChunks);
	working = true;
	renderingThread = std::thread{
		[this] {
			needTextureHandler();
			timeTotal = H::GetCurrentTimestamp<std::chrono::milliseconds>();
			lastChunkReceiveTimestamp = H::GetCurrentTimestamp<std::chrono::milliseconds>();

			while (working) {
				RenderingRoutine();
			}
		}
	};
}

void Direct2DRenderer::Stop() {
	working = false;
	renderCv.notify_one();
	lostChunksWaitingCv.notify_one();
	if (renderingThread.joinable())
		renderingThread.join();
}


void Direct2DRenderer::InitDirect2D() {
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};


	D3D_FEATURE_LEVEL m_featureLevel;

	H::System::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr,                    // specify null to use the default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,              // optionally set debug and Direct2D compatibility flags
			featureLevels,              // list of feature levels this app can support
			ARRAYSIZE(featureLevels),   // number of possible feature levels
			D3D11_SDK_VERSION,
			&d3d11Device,               // returns the Direct3D device created
			&m_featureLevel,            // returns feature level of device created
			&d3d11Context               // returns the device immediate context
		)
	);

	Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1Factory;
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d1Factory.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	H::System::ThrowIfFailed(d3d11Device->QueryInterface(dxgiDevice.GetAddressOf()));

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	H::System::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

	H::System::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

	H::System::ThrowIfFailed(d2d1Factory->CreateDevice(dxgiDevice.Get(), &d2d1Device));

	H::System::ThrowIfFailed(d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2d1DeviceContext.GetAddressOf()));


	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Format = frameConfigurationData.GetPixelFormatAsDxgiFormat();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;


	hr = dxgiFactory->CreateSwapChainForHwnd(d3d11Device.Get(), hwnd, &swapChainDesc, nullptr, nullptr, swapChain.GetAddressOf());
	H::System::ThrowIfFailed(hr);
}


void Direct2DRenderer::InitDirectRenderTarget(DXGI_FORMAT pixelFormat) {
	CreateBitmapFromSwapchain();

	D2D1_SIZE_U bitmapSize;
	bitmapSize.height = frameConfigurationData.size.height;
	bitmapSize.width = frameConfigurationData.size.width;
	auto hr = d2d1DeviceContext->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(pixelFormat, D2D1_ALPHA_MODE_PREMULTIPLIED)), remoteDesktopBitmap.GetAddressOf());
	H::System::ThrowIfFailed(hr);
	d2d1DeviceContext->SetTarget(d2d1RenderTargetBitmap.Get());

	hr = d2d1DeviceContext->CreateCompatibleRenderTarget(d2d1RenderTarget.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(D2D1::PixelFormat(pixelFormat, D2D1_ALPHA_MODE_PREMULTIPLIED));
	hr = d2d1RenderTarget->CreateBitmap(bitmapSize, props, temporaryBitmap.GetAddressOf());
	H::System::ThrowIfFailed(hr);
	d2d1DeviceContext->SetTarget(d2d1RenderTargetBitmap.Get());
}

void Direct2DRenderer::CreateBitmapFromSwapchain() {
	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;

	HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	H::System::ThrowIfFailed(hr);

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(frameConfigurationData.GetPixelFormatAsDxgiFormat(), D2D1_ALPHA_MODE_PREMULTIPLIED));

	// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
	H::System::ThrowIfFailed(d2d1DeviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, d2d1RenderTargetBitmap.GetAddressOf()));
}

namespace Test {
	std::vector<uint8_t> GetTestImage(Size frameSize, Color bgColor) {
		std::vector<uint8_t> frameData(frameSize.width * frameSize.height * 4);

		for (int h = 0; h < frameSize.height; h++) {
			for (int w = 0; w < frameSize.width; w++) {
				int pixexPos = (h * frameSize.width + w) * 4;

				Color color = bgColor;
				int offs, dx;

				offs = 50; dx = 300;
				if (offs < w && w < offs + dx)										// Left side color
					color = { 0,0,100 };

				if (frameSize.width - offs - dx < w && w < frameSize.width - offs)	// Right side color
					color = { 0,100,100 };

				//offs = 100; dx = 100;
				//if (offs < w && w < offs + dx)
				//	color = { 0,100,0 };

				//if (frameSize.width - offs - dx < w && w < frameSize.width - offs)
				//	color = { 100,100,0 };


				frameData[pixexPos + 0] = color.b;
				frameData[pixexPos + 1] = color.g;
				frameData[pixexPos + 2] = color.r;
				frameData[pixexPos + 3] = 0;
			}
		}

		return frameData;
	}
}


//void Direct2DRenderer::RenderingRoutine() {
//	// TODO: test sleep() with other capture process
//	{
//		std::unique_lock lk{ mutex };
//		//if (!renderCv.wait_for(lk, std::chrono::milliseconds(1), [this] {
//		//	//return IsAllChunksPresent() && working;
//		//	return controlChunk.data.size() > 0 && working;
//		//	})) {
//		//	return;
//		//}
//
//		if (controlChunk.data.size() == 0)
//			return;
//
//		//controlChunk = std::move(chunks[0]);
//
//		////previousFrameChunks = std::move(chunks);
//		//chunks.clear();
//		////isWaitingChunks = true; // not use
//
//		lastData = std::move(controlChunk.data);
//	}
//
//	needTextureHandler();
//	//Sleep(1);
//
//	std::unique_lock<std::mutex> lock(mutexDrawing); // guard resize swap chain
//
//	if (lastData.size() < frameConfigurationData.size.width * frameConfigurationData.size.height * 4)
//		return;
//
//	auto rect = D2D1::RectU(0, 0, frameConfigurationData.size.width, frameConfigurationData.size.height);
//
//	HRESULT hr = temporaryBitmap->CopyFromMemory(&rect, lastData.data(), 4 * frameConfigurationData.size.width);
//	H::System::ThrowIfFailed(hr);
//
//	CopyDirtyRegionsFromTempBitmapToUserDesktopBitmap();
//
//	D2D1_RECT_F dstRect = D2D1::RectF(0, 0, frameConfigurationData.size.width, frameConfigurationData.size.height);;
//
//	auto sourceBitmapSize = remoteDesktopBitmap->GetSize();
//	D2D1_RECT_F srcRect = D2D1::RectF(0, 0, sourceBitmapSize.width, sourceBitmapSize.height);
//
//	d2d1DeviceContext->BeginDraw();
//	d2d1DeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Gray));
//
//	scaleEffect->SetInput(0, remoteDesktopBitmap.Get());
//	d2d1DeviceContext->DrawImage(scaleEffect.Get());
//
//	hr = d2d1DeviceContext->EndDraw();
//	H::System::ThrowIfFailed(hr);
//
//	hr = swapChain->Present(1, 0);
//	H::System::ThrowIfFailed(hr);
//}


void Direct2DRenderer::RenderingRoutine() {
	if (!resizing) {
		std::vector<std::uint8_t> data;

		{
			std::unique_lock lk{ mutex };
			if (!renderCv.wait_for(lk, std::chrono::milliseconds{ renderingDelay }, [this] {
				return IsAllChunksPresent() && working;
				})) {
				return;
			}

			for (auto& chunk : chunks) {
				data.insert(data.end(), chunk.data.begin(), chunk.data.end());
			}
			controlChunk = chunks[0];
			previousFrameChunks = std::move(chunks);
			chunks.clear();
			isWaitingChunks = true;
		}


		std::lock_guard<std::mutex> lock(mutexDrawing); // guard resize swap chain

		//TODO move chunk request to thread waiter, add bool for waiting new chunks
		needTextureHandler();

		auto decodedData = std::move(data);
		if (decodedData.size() < frameConfigurationData.size.width * frameConfigurationData.size.height * 4)
			return;

		auto rect = D2D1::RectU(0, 0, frameConfigurationData.size.width, frameConfigurationData.size.height);

		HRESULT hr = temporaryBitmap->CopyFromMemory(&rect, decodedData.data(), 4 * frameConfigurationData.size.width);
		H::System::ThrowIfFailed(hr);

		CopyDirtyRegionsFromTempBitmapToUserDesktopBitmap();

		d2d1DeviceContext->BeginDraw();
		d2d1DeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		scaleEffect->SetInput(0, remoteDesktopBitmap.Get());
		rotateEffect->SetInput(0, remoteDesktopBitmap.Get());
		rotateEffect->SetInputEffect(0, scaleEffect.Get());

		d2d1DeviceContext->DrawImage(rotateEffect.Get());

		hr = d2d1DeviceContext->EndDraw();
		H::System::ThrowIfFailed(hr);

		hr = swapChain->Present(1, 0);
		H::System::ThrowIfFailed(hr);

		lastData = std::move(decodedData);
	}
	else { // Use last saved frame while resizing
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); // wait some small time for thread (to avoid max load current drawing thread)

		std::lock_guard<std::mutex> lock(mutexDrawing); // guard resize swap chain

		if (lastData.size() < frameConfigurationData.size.width * frameConfigurationData.size.height * 4)
			return;

		auto rect = D2D1::RectU(0, 0, frameConfigurationData.size.width, frameConfigurationData.size.height);

		HRESULT hr = temporaryBitmap->CopyFromMemory(&rect, lastData.data(), 4 * frameConfigurationData.size.width);
		H::System::ThrowIfFailed(hr);

		CopyDirtyRegionsFromTempBitmapToUserDesktopBitmap();

		d2d1DeviceContext->BeginDraw();
		d2d1DeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		scaleEffect->SetInput(0, remoteDesktopBitmap.Get());
		rotateEffect->SetInput(0, remoteDesktopBitmap.Get());
		rotateEffect->SetInputEffect(0, scaleEffect.Get());

		d2d1DeviceContext->DrawImage(rotateEffect.Get());

		hr = d2d1DeviceContext->EndDraw();
		H::System::ThrowIfFailed(hr);

		hr = swapChain->Present(1, 0);
		H::System::ThrowIfFailed(hr);
	}
}

void Direct2DRenderer::ChunkWaitingRoutine() { // [not used]
	auto needNewChunk = [this] {
		auto timestamp = H::GetCurrentTimestamp<std::chrono::milliseconds>() - lastChunkReceiveTimestamp > renderingDelay * 10;
		return H::GetCurrentTimestamp<std::chrono::milliseconds>() - lastChunkReceiveTimestamp > renderingDelay * 10 && !isWaitingChunks && working;
		//return !isWaitingChunks;
	};

	std::unique_lock lk{ mutex };
	if (lostChunksWaitingCv.wait_for(lk, std::chrono::milliseconds{ renderingDelay }, needNewChunk)) {
		std::vector<uint8_t> lostChunks;
		if (chunks.size() == 0) {
			lostChunks.push_back(0);
			//lostFrameHandler(lostChunks, lastFrameTimestamp);
			//needTextureHandler();
			return;
		}
		//std::unique_lock lk1{ mutex };
		if (!IsAllChunksPresent()) {
			//needTextureHandler();
			for (size_t i = 0; i < chunks.size(); ++i) {
				if (chunks[i].data.size() == 0)
					lostChunks.push_back(i);
			}
			//lostFrameHandler(lostChunks, lastFrameTimestamp);
		}
	}
	else if (isWaitingChunks) {
		if (resend == 20) {
			resend = 0;
			isWaitingChunks = false;
		}
		else
			++resend;
	}
}

bool Direct2DRenderer::IsAllChunksPresent() {
	// mutex locked by caller
	if (chunks.size() == 0) {
		if (H::GetCurrentTimestamp<std::chrono::milliseconds>() - lastChunkReceiveTimestamp > 8'000) {
			lastChunkReceiveTimestamp = H::GetCurrentTimestamp<std::chrono::milliseconds>();
			OutputDebugStringA("--- Direct2DRenderer: Try request frame again !!! --- \n");
			needTextureHandler();
		}
		return false;
	}

	for (auto& chunk : chunks) {
		if (chunk.data.size() == 0)
			return false;
	}
	renderCv.notify_one();
	return true;
}

void Direct2DRenderer::CopyDirtyRegionsFromTempBitmapToUserDesktopBitmap() {
	for (auto& region : controlChunk.dirtyRegions) {
		D2D1_RECT_U rect;
		rect.bottom = region.y + region.height;
		rect.top = region.y;
		rect.right = region.x + region.width;
		rect.left = region.x;

		D2D1_POINT_2U destinationPoint;
		destinationPoint.x = region.x;
		destinationPoint.y = region.y;

		remoteDesktopBitmap->CopyFromBitmap(&destinationPoint, temporaryBitmap.Get(), &rect);
	}
}