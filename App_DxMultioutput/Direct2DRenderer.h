#pragma once
#include <d2d1_1.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wrl.h>
#include <limits>
#include <vector>
#include <functional>
#include <condition_variable>
#include "Frame.h"

class Direct2DRenderer {
public:
	~Direct2DRenderer();

	void SetNeedTextureHandler(std::function<void()> handler);
	void SetLostFrameRequestHandler(std::function<void(std::vector<uint8_t>, uint64_t)> handler);
	void SetNewFrameChunksCount(uint8_t totalChunks);

	void SetDecodeHandler(std::function< std::vector<uint8_t>(std::vector<uint8_t>&)> handler);
	void AddFrameChunk(Frame&& chunk, uint8_t index, uint8_t chunksCount);
	void SetFrameConfiguration(FrameConfigurationData frameConfiguration);
	void SetHwnd(HWND hwnd);
	void SetFps(uint8_t fps);

	void Resizing(bool enable);
	void SetWindowSize(Size newSize);

	void Start();
	void Stop();

	//std::vector<Color> testColors;

private:
	void InitDirect2D();
	void InitDirectRenderTarget(DXGI_FORMAT pixelFormat);
	void CreateBitmapFromSwapchain();

	void RenderingRoutine();
	void ChunkWaitingRoutine();

	bool IsAllChunksPresent();
	void CopyDirtyRegionsFromTempBitmapToUserDesktopBitmap();

private:
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

	FrameConfigurationData frameConfigurationData = { 0 };

	std::atomic<float> scaleX = 1;
	std::atomic<float> scaleY = 1;
	std::atomic<double> aspectRatio = 0;

	std::function<void()> needTextureHandler;
	std::function<void(std::vector<uint8_t>, uint64_t)> lostFrameHandler;
	std::function< std::vector<uint8_t>(std::vector<uint8_t>&)> decodeHandler;

	std::atomic<bool> isWaitingChunks = true;
	std::atomic<uint64_t> lastChunkReceiveTimestamp;
	std::mutex mutex;
	std::condition_variable renderCv;
	std::condition_variable lostChunksWaitingCv;

	std::vector<Frame> chunks;
	std::vector<Frame> previousFrameChunks;
	Frame controlChunk;


	uint64_t renderingDelay = 17; // milliseconds 
	bool working = false;

	int resend = 0;

	std::thread lostChunkWaitingThread;
	std::thread renderingThread;

	//FPS display
	int numFrames = 0;
	float timeTotal;
	Microsoft::WRL::ComPtr<IDWriteFactory> directWriteFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> blackBrush;


	std::mutex mutexRenderTarget;
	std::mutex mutexDrawing;

	std::vector<std::uint8_t> lastData;
	std::atomic<bool> resizing = false;
	std::atomic<bool> promptImageDrawing = false;
	std::atomic<bool> canResizeSwapChain = false;
};


namespace Test {
	std::vector<uint8_t> GetTestImage(Size frameSize, Color bgColor = { 100, 100, 100 });
}