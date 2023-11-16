#pragma once

struct Size {
	unsigned int width;
	unsigned int height;

	bool operator != (const Size& other) {
		return width != other.width || height != other.height;
	}
};