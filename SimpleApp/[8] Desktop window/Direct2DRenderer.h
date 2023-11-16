#pragma once
#include "IRenderer.h"
#include "Thread.h"
#include <condition_variable>
#include <d2d1_1.h>
#include <wrl.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <limits>

#include "Size.h"
#include "Frame.h"
#include "CursorInfo.h"
#include "CursorMapper.h"
#include "FrameConfigurationData.h"

namespace Desktop {
	class Direct2DRenderer final : public IRenderer, public H::IThread {
	public:
		Direct2DRenderer() = default;
		~Direct2DRenderer();

		// IRenderer
		void SetNeedTextureHandler(std::function<void()> handler) final;
		void SetLostFrameRequestHandler(std::function<void(std::vector<uint8_t>, uint64_t)> handler) final;
		void SetNewFrameChunksCount(uint8_t totalChunks) final;

		void SetServerCursor(const CursorInfo& cursorInfo);

		void SetDecodeHandler(std::function< std::vector<uint8_t>(std::vector<uint8_t>&)> handler) final;
		void AddFrameChunk(Frame&& chunk, uint8_t index, uint8_t chunksCount) final;
		void SetFrameConfiguration(FrameConfigurationData frameConfiguration) final;
		void SetHwnd(HWND hwnd) final;
		void SetFps(uint8_t fps) final;

		void Resizing(bool enable) final;
		void SetWindowSize(Size newSize) final;
		void UseStaticImage(bool value) final;
		void UpdateTexture() final;

		void Start() final;
		void Stop() final;

		// IThread
		void NotifyAboutStop() override;
		void WaitingFinishThreads() override;

	private:
		void InitDirect2D();
		void InitDirectRenderTarget(DXGI_FORMAT pixelFormat);
		void CreateBitmapFromSwapchain();

		void RenderingRoutine();

		bool IsAllChunksPresent();
		void CopyDirtyRegionsFromTempBitmapToUserDesktopBitmap();

	private:
		std::mutex mutex;
		std::mutex mutexDrawing;
		std::mutex mutexRenderTarget;

		HWND hwnd;
		Microsoft::WRL::ComPtr<ID2D1Device> d2d1Device;
		Microsoft::WRL::ComPtr<ID3D11Device> d3d11Device;
		Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11Context;

		Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> d2d1RenderTarget;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d1RenderTargetBitmap;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> remoteDesktopBitmap;
		Microsoft::WRL::ComPtr<ID2D1Bitmap> temporaryBitmap;
		Microsoft::WRL::ComPtr<ID2D1Effect> scaleEffect;
		Microsoft::WRL::ComPtr<ID2D1Effect> rotateEffect;

		std::pair<int, int> remoteCursorPos;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> remoteCursorBitmap;
		CursorMapper cursorMapper;
		Microsoft::WRL::ComPtr<ID2D1Effect> remoteCursorPositionEffect;
		Microsoft::WRL::ComPtr<ID2D1Effect> remoteCursorTintEffect;

		Frame controlChunk;
		std::vector<Frame> chunks;
		std::vector<Frame> previousFrameChunks;
		std::vector<std::uint8_t> lastData;
		FrameConfigurationData frameConfigurationData = { 0 };


		//FPS display
		Microsoft::WRL::ComPtr<IDWriteFactory> directWriteFactory;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> blackBrush;

		uint64_t renderingDelay = 17; // milliseconds 

		std::atomic<bool> working = false;
		std::atomic<bool> resizing = false;
		std::atomic<bool> useStaticImage = false;
		std::atomic<bool> promptImageDrawing = false;
		std::atomic<bool> canResizeSwapChain = false;
		std::atomic<int> numFrames = 0;
		std::atomic<float> scaleX = 1;
		std::atomic<float> scaleY = 1;
		std::atomic<double> aspectRatio = 0;
		std::atomic<uint64_t> lastChunkReceivedTimestamp;

		std::function<void()> needTextureHandler;
		std::function<void(std::vector<uint8_t>, uint64_t)> lostFrameHandler;
		std::function< std::vector<uint8_t>(std::vector<uint8_t>&)> decodeHandler;

		std::thread renderingThread;
		std::condition_variable renderCv;
	};
}