#include "RenderingNode.h"
#include <cassert>

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


//#define LOG_DEBUG(formatStr, ...)


	RenderingNode::RenderingNode()
		: dxRenderer{ std::make_shared<Desktop::Direct2DRenderer>() }
	{
	}

	RenderingNode::~RenderingNode() {
		//LOG_DEBUG("~RenderingNode() start ...");
		// Register all thread-classes(that inherited from IThread) to be completed automatically:
		threadsFinishHelper.Register(dxRenderer, L"RenderingNode->dxRenderer");

		threadsFinishHelper.Stop(); // first notified all registered thread-classes about stop then waiting each of them
		//LOG_DEBUG("registered thread-classes should already be completed");

		// ensure to finish lambda before other class members start destroying ...
		if (renderingRoutine.valid())
			renderingRoutine.wait();
		//LOG_DEBUG("~RenderingNode() end");
	}

	void RenderingNode::Init(HWND hwnd, FrameConfigurationData frameConfiguration, std::vector<Color> testColors) {
		this->testColors = testColors;
		this->currentColor = this->testColors.begin();
		this->frameConfiguration = frameConfiguration;

		dxRenderer->SetFrameConfiguration(frameConfiguration);
		dxRenderer->SetHwnd(hwnd);
		dxRenderer->SetWindowSize(frameConfiguration.size);
	}

	Desktop::Direct2DRenderer* RenderingNode::GetDxRenderer() {
		return dxRenderer.get();
	}

	void RenderingNode::Start() {
		dxRenderer->SetNeedTextureHandler([this] {
			try {
				// [needTextureHandler call 2] here dxRenderer mutex still locked
				if (renderingRoutine.valid())
					renderingRoutine.get();
			}
			catch (...) {
				assert(false && "Test rendering node exception !!!");
			}


			renderingRoutine = std::async(std::launch::async, [this] {

				auto lk = std::unique_lock<std::mutex>{ mxFrame, std::defer_lock };
				if (!lk.try_lock()) {
					// Texture handler already executing ...
					return;
				}

				Sleep(333);
				Frame frame;
				frame.data = GetTestImage(frameConfiguration.size, *currentColor);
				frame.dirtyRegions.push_back({ 0,0, frameConfiguration.size.width, frameConfiguration.size.height });

				// BUG: deadlock
				// [needTextureHandler call 1] AddFrameChunk may lock dxRenderer mutex in same thread where was [needTextureHandler call 2]
				dxRenderer->AddFrameChunk(std::move(frame), 0, 1);
				if (++currentColor == testColors.end()) {
					currentColor = testColors.begin();
				}
				});
			});
		dxRenderer->Start();
	}

	void RenderingNode::Stop() {
		dxRenderer->Stop();
		dxRenderer->SetNeedTextureHandler([this] {});
	}
}