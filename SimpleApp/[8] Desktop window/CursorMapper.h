#pragma once
#include <map>
#include "Bimap.hpp"

namespace Desktop {
	enum class CursorIcon {
		Unknown = 0,
		Arrow,
		UpArrow,
		Cross,
		Wait,
		IBeam,
		SizeVer,
		SizeHor,
		SizeBDiag,
		SizeFDiag,
		SizeAll,
		//Blank,
		//SplitV,
		//SplitH,
		PointingHand,
		Forbidden,
		//OpenHand,
		//ClosedHand,
		WhatsThis,
		Bussy,
	};


	class CursorMapper {
	public:
		CursorMapper();
		~CursorMapper() = default;

		CursorIcon WindowsCursorToUniversal(int hCursor);
		int UniversalCursorToWindows(CursorIcon cursorIcon);

	private:
		H::Bimap<CursorIcon, int> cursorBimapper;
	};
}