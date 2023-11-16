#pragma once
#include <vector>

//namespace Desktop {
	struct CursorInfo {
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		int pitch = 0;
		int xHotspot = 0;
		int yHotspot = 0;
		int hCursor = 0;
		std::vector<uint8_t> image;

		inline void CancelMovement() {
			x = y = -1;
		}

		inline bool IsMovedByServer() const {
			return x > -1 && y > -1;
		}
	};
//}