#pragma once
#include <vector>

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