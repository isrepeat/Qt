#pragma once
#include <vector>
#include <cstdint> // uint32_t
#include <dxgi1_2.h>

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

struct Size {
	unsigned int width;
	unsigned int height;

	bool operator != (const Size& other) {
		return width != other.width || height != other.height;
	}
};

struct FrameConfigurationData {
	DpiData displayData;
	PixelFormat pixelFormat;
	DesktopOrientation desktopOrientation = DesktopOrientation::Landscape;
	Size size;
	int startX;
	int startY;

	DXGI_FORMAT GetPixelFormatAsDxgiFormat() {
		switch (pixelFormat)
		{
		case PixelFormat::BGRA32:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
};

struct DirtyImageRegion {
	int32_t x = 0;
	int32_t y = 0;
	uint32_t width = 0;
	uint32_t height = 0;
};

struct MovedImageRegion {
	int32_t sourceX = 0;
	int32_t sourceY = 0;

	int32_t destinationX = 0;
	int32_t destinationY = 0;

	uint32_t width = 0;
	uint32_t height = 0;
};

struct Frame {
	std::vector<std::uint8_t> data;
	std::vector<DirtyImageRegion> dirtyRegions;
	std::vector<MovedImageRegion> movedRegions;
	uint8_t outputIndex = 0;
};