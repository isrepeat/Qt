#pragma once
#include "Direct2DRenderer.h"
#include <future>

struct Color {
	int b, g, r;
};

namespace Test {
	class RenderingNode {
	public:
		RenderingNode();
		~RenderingNode();

		void Init(HWND hwnd, FrameConfigurationData frameConfiguration, std::vector<Color> testColors);
		Desktop::Direct2DRenderer* GetDxRenderer();
		void Start();
		void Stop();

	private:

		FrameConfigurationData frameConfiguration;
		std::mutex mxFrame;
		std::future<void> renderingRoutine;

		std::vector<Color> testColors;
		std::vector<Color>::iterator currentColor;
		std::shared_ptr<Desktop::Direct2DRenderer> dxRenderer;
		
		H::ThreadsFinishHelper threadsFinishHelper;
	};
}
