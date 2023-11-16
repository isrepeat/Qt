#include "FrameConfigurationData.h"

//namespace Desktop {
	DXGI_FORMAT FrameConfigurationData::GetPixelFormatAsDxgiFormat() {
		switch (pixelFormat)
		{
		case PixelFormat::BGRA32:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
//}