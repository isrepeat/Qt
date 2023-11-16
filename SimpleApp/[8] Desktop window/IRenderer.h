#pragma once
#include <functional>
#include "Frame.h"
#include "FrameConfigurationData.h"
#include <d2d1_1.h>
#include <dxgi1_2.h>
#include <d3d11.h>

namespace Desktop {
	/// <summary>
	/// Interface for object which renders VNC server screen. In order to render requires texture(Frame). After frame rendering sends request for next frame to remote side(VNC server). 
	/// </summary>
	class IRenderer {
	public:

		/// <summary>
		/// Set handler which is called each time object needs new frame. Should be set before start rendering
		/// </summary>
		virtual void SetNeedTextureHandler(std::function<void()> handler) = 0;
		virtual void SetLostFrameRequestHandler(std::function<void(std::vector<uint8_t>, uint64_t)> handler) = 0;
		virtual void SetNewFrameChunksCount(uint8_t totalChunks) = 0;

		virtual void SetServerCursor(const CursorInfo& cursorInfo) = 0;

		/// <summary>
		/// Set handler which is called each time object got new frame and need to decode it. Should be set before start rendering
		/// </summary>
		virtual void SetDecodeHandler(std::function< std::vector<uint8_t>(std::vector<uint8_t>&)> handler) = 0;

		/// <summary>
		/// Set new frame chunk to object. If raw UDP is used one package is limited in size, so we divide frame into chunks
		/// </summary>
		virtual void AddFrameChunk(Frame&& chunk, uint8_t index, uint8_t chunksCount) = 0;

		virtual void SetFrameConfiguration(FrameConfigurationData frameConfiguration) = 0;

		/// <summary>
		/// Set direct2d components allocated in ui app due to issue with usage of hwnd not from parent thread. 
		/// Device and context initialization can be moved inside library, only swap chain create in UI thread
		/// </summary>
		//virtual void SetDirect2DComponents(ID3D11Device* device, IDXGISwapChain1* swapChain, ID2D1DeviceContext* context) = 0;
		virtual void SetHwnd(HWND hwnd) = 0;
		virtual void SetFps(uint8_t fps) = 0;
		virtual void Resizing(bool enable) = 0;
		virtual void SetWindowSize(Size newSize) = 0;
		virtual void UseStaticImage(bool value) = 0;
		virtual void UpdateTexture() = 0;

		virtual void Start() = 0;
		virtual void Stop() = 0;

		virtual ~IRenderer() = default;
	};
}