#pragma once
#include "Direct2DRenderer.h"
#include "RenderWindow.h"

class RenderingNode {
public:
	RenderingNode();
	RenderWindow* GetRenderWindow();

	void Init(FrameConfigurationData frameConfiguration, std::vector<Color> testColors);

private:
	std::vector<Color> testColors;
	std::vector<Color>::iterator currentColor;
    std::unique_ptr<RenderWindow> renderWindow;
    std::unique_ptr<Direct2DRenderer> dxRenderer;
};
