#pragma once
#include "Size.h"
#include "CursorInfo.h"

#include <cstdint> // uint32_t
#include <dxgi1_2.h>

//namespace Desktop {
	enum class PixelFormat {
		BGRA32
	};

	enum class DesktopOrientation {
		Landscape,
		Portrait,
		LandscapeFlipped,
		PortraitFlipped
	};

	struct DpiData {
		uint32_t dpiX;
		uint32_t dpiY;
	};

	struct FrameConfigurationData {
		DpiData displayData;
		PixelFormat pixelFormat;
		DesktopOrientation desktopOrientation = DesktopOrientation::Landscape;
		Size size;
		int startX;
		int startY;

		DXGI_FORMAT GetPixelFormatAsDxgiFormat();
	};

	struct SerializedFrame {
		std::vector<uint8_t> frameData;
		CursorInfo cursorInfo;
	};
//}