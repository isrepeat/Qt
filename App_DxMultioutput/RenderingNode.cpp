#include "RenderingNode.h"

RenderingNode::RenderingNode()
	: renderWindow{ new RenderWindow() }
	, dxRenderer{ new Direct2DRenderer() }
{
}

RenderWindow* RenderingNode::GetRenderWindow() {
	return renderWindow.get();
}

void RenderingNode::Init(FrameConfigurationData frameConfiguration, std::vector<Color> testColors) {
	this->testColors = testColors;
	this->currentColor = this->testColors.begin();

	dxRenderer->SetFrameConfiguration(frameConfiguration);
	dxRenderer->SetNeedTextureHandler([this, frameConfiguration] {
		std::thread([this, frameConfiguration] {
			Sleep(500);
			Frame frame;
			frame.data = Test::GetTestImage(frameConfiguration.size, *this->currentColor);
			frame.dirtyRegions.push_back({ 0,0, frameConfiguration.size.width, frameConfiguration.size.height });

			dxRenderer->AddFrameChunk(std::move(frame), 0, 1);
			if (++this->currentColor == this->testColors.end()) {
				this->currentColor = this->testColors.begin();
			}
			}).detach();
		});
	dxRenderer->SetHwnd((HWND)renderWindow->winId());
	dxRenderer->SetWindowSize(frameConfiguration.size);
	dxRenderer->Start();
}
