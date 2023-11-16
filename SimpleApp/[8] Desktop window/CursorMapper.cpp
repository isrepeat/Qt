#include "CursorMapper.h"
#include <Windows.h>


namespace Desktop {
	// NOTE: LoadCursor return device dependent codes, so you must initialize cursorMap for client and server 
	//       (don't use it on just one side)
	std::map<CursorIcon, int> GetCursorMap() {
		std::map<CursorIcon, int> cursorMap;
		cursorMap[CursorIcon::Arrow] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_ARROW));
		cursorMap[CursorIcon::UpArrow] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_UPARROW));
		cursorMap[CursorIcon::Cross] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_CROSS));
		cursorMap[CursorIcon::Wait] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_WAIT));
		cursorMap[CursorIcon::IBeam] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_IBEAM));
		cursorMap[CursorIcon::SizeVer] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_SIZENS));
		cursorMap[CursorIcon::SizeHor] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_SIZEWE));
		cursorMap[CursorIcon::SizeBDiag] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_SIZENESW));
		cursorMap[CursorIcon::SizeFDiag] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_SIZENWSE));
		cursorMap[CursorIcon::SizeAll] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_SIZEALL));
		//cursorMap[CursorIcon::Blank] = reinterpret_cast<int>(LoadCursorW(NULL, ???));
		//cursorMap[CursorIcon::SplitV] = reinterpret_cast<int>(LoadCursorW(NULL, ???));
		//cursorMap[CursorIcon::SplitH] = reinterpret_cast<int>(LoadCursorW(NULL, ???));
		cursorMap[CursorIcon::PointingHand] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_HAND));
		cursorMap[CursorIcon::Forbidden] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_NO));
		//cursorMap[CursorIcon::OpenHand] = reinterpret_cast<int>(LoadCursorW(NULL, ???));
		//cursorMap[CursorIcon::ClosedHand] = reinterpret_cast<int>(LoadCursorW(NULL, ???));
		cursorMap[CursorIcon::WhatsThis] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_HELP));
		cursorMap[CursorIcon::Bussy] = reinterpret_cast<int>(LoadCursorW(NULL, IDC_APPSTARTING));

		return cursorMap;
	}


	CursorMapper::CursorMapper() 
		: cursorBimapper{ GetCursorMap() }
	{
	}

	CursorIcon CursorMapper::WindowsCursorToUniversal(int hCursor) {
		return cursorBimapper[hCursor];
	}

	int CursorMapper::UniversalCursorToWindows(CursorIcon cursorIcon) {
		return cursorBimapper[cursorIcon];
	}
}