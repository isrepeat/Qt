#include "MainWindow.h"
#include <Windows.h>
#include <QByteArray>
#include <QMimeData>
#include <QProcess>
#include <QLayout>
#include <QBuffer>
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QtWin>
#include <thread>
#include <vector>

#include <fstream>
#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;



struct CursorInfo {
	int hCursor = 0;
	std::vector<uint8_t> image;
};

std::vector<uint8_t> PixmapToVectorBytes(const QPixmap& pixmap) {
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	return { bytes.begin(), bytes.end() };
}

QPixmap PixmapFromVectorBytes(const std::vector<uint8_t>& bytes) {
	QPixmap pixmap;
	pixmap.loadFromData(bytes.data(), bytes.size());
	return pixmap;
}

//CursorInfo g_cursorInfo;


MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent)
	, dpiScaleFactorSystem{ GetDpiForSystem() / 96.0f }
{
	// init GdiPlus
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 2px solid green;}");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	label = new QLabel(this);
	label->setStyleSheet("QLabel{border: 2px solid red; background-color: gray;}");
	vLayout->addWidget(label);

	pushBtn = new QPushButton("Switch cursor");
	//pushBtn->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	vLayout->addWidget(pushBtn);


	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [this] {
		//auto cursorInfo = GetCursorInfo();
		//label->setPixmap(PixmapFromVectorBytes(cursorInfo.image));

		//auto pixmapCursor = GetCursorInfo();

		if (showNativeCursor) {
			GetCursorInfo();
		}
		else {
			QCursor cursror(this->myCursor.pixmapCursor, this->myCursor.hotX, this->myCursor.hotY);
			this->setCursor(cursror);
		}

		label->setPixmap(this->myCursor.pixmapCursor);
		});

	timer->start(2000);
	

	//QWidget* innerWidget = new QWidget(this);
	//innerWidget->setStyleSheet("QWidget{border: 2px solid red;}");
	//innerWidget->setFixedHeight(200);
	//vLayout->addWidget(innerWidget);

	//auto vvLayout = new QVBoxLayout(innerWidget);
	//auto hhLayout = new QHBoxLayout();
	//auto hhLayout2 = new QHBoxLayout();


	//auto lbA = new QLabel();
	////lbA->setPixmap(QPixmap("D:\\icon.ico"));
	//lbA->setPixmap(getMouseCursorIconWin());
	////lbA->setStyleSheet("QLabel{color: blue; border: 2px solid blue;}");
	////hhLayout->setAlignment(Qt::AlignBottom);
	//hhLayout->addWidget(lbA);



	////vvLayout->addSpacing(100);
	//vvLayout->addLayout(hhLayout);
	//vvLayout->addLayout(hhLayout2);


	connect(pushBtn, &QPushButton::clicked, [this] {
		showNativeCursor = !showNativeCursor;
		pushBtn->setText(QString{ "showNativeCursor = %1" }.arg(showNativeCursor ? "true" : "false"));
		});
}

MainWindow::~MainWindow() {
	GdiplusShutdown(gdiplusToken);
}




// get image encoder CLSID by its name
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	unsigned int num = 0, size = 0;
	GetImageEncodersSize(&num, &size);
	if (size == 0) return -1;
	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL) return -1;
	GetImageEncoders(num, size, pImageCodecInfo);
	for (unsigned int j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}


void HBitmapToPng(HBITMAP hBitmap) {
	std::vector<unsigned char> buffer;

	// create Gdiplus::Bitmap object
	Bitmap* pScreenShot = new Bitmap(hBitmap, (HPALETTE)NULL);

	// encoding params
	ULONG quality = 50;
	EncoderParameters encoderParams;
	encoderParams.Count = 1;
	encoderParams.Parameter[0].NumberOfValues = 1;
	encoderParams.Parameter[0].Guid = EncoderQuality;
	encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParams.Parameter[0].Value = &quality;
	CLSID imageCLSID;
	GetEncoderClsid(L"image/jpeg", &imageCLSID);

	// create empty stream
	IStream* pIStream = NULL;
	bool ok = (CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&pIStream) == S_OK);
	if (ok) {
		// save imaget into it
		ok = (pScreenShot->Save(pIStream, &imageCLSID, &encoderParams) == Ok);
		if (ok) {
			// get the size of the stream
			ULARGE_INTEGER ulnSize;
			LARGE_INTEGER lnOffset;
			ULONG ulBytesRead = 0;
			lnOffset.QuadPart = 0;
			ok = (pIStream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize) == S_OK);
			if (ok) {
				// now move the pointer to the beginning of the file
				ok = (pIStream->Seek(lnOffset, STREAM_SEEK_SET, NULL) == S_OK);
				if (ok) {
					buffer.resize(ulnSize.QuadPart);
					ok = (pIStream->Read(buffer.data(), buffer.size(), &ulBytesRead) == S_OK);
				}
			}
			pIStream->Release();
		}
	}

	delete pScreenShot;


	std::ofstream fout("file.png", std::ios::out | std::ios::binary);
	fout.write((char*)buffer.data(), buffer.size());
	fout.close();
}


BOOL DrawBitmap(HDC hDC, INT x, INT y, INT width, INT height, HBITMAP hBitmap, DWORD dwROP)
{
	HDC       hDCBits;
	BITMAP    Bitmap;
	BOOL      bResult;

	if (!hDC || !hBitmap)
		return FALSE;

	hDCBits = CreateCompatibleDC(hDC);
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	SelectObject(hDCBits, hBitmap);
	// Replace with StretchBlt call
	//bResult = BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hDCBits, 0, 0, dwROP);
	bResult = StretchBlt(hDC, x, y, width, height, hDCBits, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, dwROP);
	DeleteDC(hDCBits);

	return bResult;
}


HBITMAP ResizeBitmap(HDC hDC, HBITMAP source)
{
	HDC hMemDC2 = CreateCompatibleDC(hDC);
	HGDIOBJ hOld2 = SelectObject(hMemDC2, source);

	BITMAP bitmap = { 0 };
	GetObject(source, sizeof(BITMAP), &bitmap);

	float SCALE = 1;
	int nWidth = (int)ceil(bitmap.bmWidth / SCALE / 32) * 32;
	int nHeight = (int)ceil(bitmap.bmHeight / SCALE / 32) * 32;
	int dWidth = (int)round(bitmap.bmWidth / SCALE);
	int dHeight = (int)round(bitmap.bmHeight / SCALE);

	HDC hMemDC1 = CreateCompatibleDC(hDC);
	HBITMAP hBitmap1 = CreateCompatibleBitmap(hDC, nWidth, nHeight);
	HGDIOBJ hOld1 = SelectObject(hMemDC1, hBitmap1);

	StretchBlt(hMemDC1, 0, 0, dWidth, dHeight, hMemDC2, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

	SelectObject(hMemDC1, hOld1);
	SelectObject(hMemDC2, hOld2);
	DeleteDC(hMemDC1);
	DeleteDC(hMemDC2);

	return hBitmap1;
}




QPixmap MainWindow::GetCursorInfo() {
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	::GetCursorInfo(&ci);


	auto logicalDpiX = this->logicalDpiX();
	auto logicalDpiY = this->logicalDpiY();
	auto physicalDpiX = this->physicalDpiX();
	auto physicalDpiY = this->physicalDpiY();
	auto dpi = devicePixelRatio();
	auto qtDpi = devicePixelRatioF();
	float dtDpi = qtDpi / dpiScaleFactorSystem;

	int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
	int cursorHeight = GetSystemMetrics(SM_CYCURSOR);



	// Get your device contexts.
	HDC hdcScreen = GetDC(NULL);
	//HDC hdcScreen = GetDC((HWND)this->winId());
	//HDC hdcMem = CreateCompatibleDC(hdcScreen);	
	//HDC hdcMem = CreateCompatibleDC(NULL);
	


	//// Draw the cursor into the canvas.
	//DrawIcon(hdcMem, 0, 0, ci.hCursor);
	////DrawIconEx(hdcMem, 0, 0, ci.hCursor, 256, 256, 0, nullptr, DI_NORMAL);

	auto ii = CopyIcon(ci.hCursor);
	ICONINFO iconinfo;
	if (GetIconInfo(ii, &iconinfo)) {
		//HGDIOBJ hbmOld = SelectObject(hdcMem, iconinfo.hbmColor);

		//BITMAP Bmp = { 0 };
		//GetObject(iconinfo.hbmColor, sizeof(Bmp), &Bmp);

		QPixmap cursorPixmap = QtWin::fromHBITMAP(iconinfo.hbmColor, QtWin::HBitmapNoAlpha);

		auto image = PixmapToVectorBytes(cursorPixmap);
		auto pixmap = PixmapFromVectorBytes(image);
		
		auto w = pixmap.width();
		auto h = pixmap.height();

		this->myCursor.hotX = iconinfo.xHotspot / 3;
		this->myCursor.hotY = iconinfo.yHotspot / 3;
		this->myCursor.pixmapCursor = pixmap.scaled(w / 3, h / 3, Qt::IgnoreAspectRatio);
		
		qDebug() << "pixmap = " << this->myCursor.pixmapCursor;
		qDebug() << "hotXY = [" << iconinfo.xHotspot << ";" << iconinfo.yHotspot << "]";

		//HBITMAP hBitmap =  QtWin::toHBITMAP(this->pixmapCursor, QtWin::HBitmapNoAlpha);

		//BITMAP Bmp = { 0 };
		//GetObject(hBitmap, sizeof(Bmp), &Bmp);

		//HDC hdcMem = CreateCompatibleDC(NULL);
		//HDC hdcDest = CreateCompatibleDC(NULL);
		//HBITMAP newHBitmap = CreateCompatibleBitmap(hdcScreen, Bmp.bmWidth / 2, Bmp.bmHeight / 2);
		//HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);
		//HGDIOBJ hbmOld_2 = SelectObject(hdcDest, newHBitmap);

		//////HBITMAP hBmpCopy = (HBITMAP)CopyImage(iconinfo.hbmColor, IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);

		////HGDIOBJ hmbOld_2 = SelectObject(hdcDest, newHBitmap);

		//////HBITMAP newHBitmap = ResizeBitmap(NULL, iconinfo.hbmColor);
		//SetStretchBltMode(hdcMem, HALFTONE);
		////auto bResult = BitBlt(hdcScreen, 0, 0, 24, 24, hdcMem, 0, 0, SRCCOPY);
		////auto bResult = StretchBlt(hdcScreen, 0, 0, 64, 64, hdcMem, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
		//auto bResult = StretchBlt(hdcDest, 0, 0, 32, 32, hdcMem, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
		//////auto bResult = StretchBlt(hdcDest, 0, 0, 24, 24, hdcMem, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);

		//////HBitmapToPng(iconinfo.hbmColor);

		////qDebug() << "hotXY = [" << iconinfo.xHotspot << ";" << iconinfo.yHotspot << "]";

		////// Convert to QPixmap.
		//////QPixmap cursorPixmap = QtWin::fromHBITMAP(iconinfo.hbmColor, QtWin::HBitmapNoAlpha);
		//////QPixmap cursorPixmap = QtWin::fromHBITMAP(newHBitmap, QtWin::HBitmapNoAlpha);
		//////QPixmap cursorPixmap = QtWin::fromHBITMAP(hBmpCopy, QtWin::HBitmapNoAlpha);
		//

		//this->pixmapCursor = QtWin::fromHBITMAP(newHBitmap, QtWin::HBitmapNoAlpha);


		////SelectObject(hdcDest, hmbOld_2);
		////DeleteObject(newHBitmap);
		////DeleteDC(hdcDest);

		//SelectObject(hdcDest, hbmOld_2);
		//SelectObject(hdcMem, hbmOld);
		//DeleteObject(newHBitmap);
		//DeleteDC(hdcDest);
		//DeleteDC(hdcMem);
	}


	// Clean up after yourself.
	//DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
	return {};
}


void MainWindow::paintEvent(QPaintEvent* event) {
	//QRect rcBounds = event->rect();
	//QPainter tmp(this);

	////tmp.begin(this);
	////tmp.drawPixmap(rcBounds.left(), rcBounds.top(), this->pixmapCursor, rcBounds.left(), rcBounds.top(), rcBounds.width(), rcBounds.height());
	//tmp.drawPixmap(rcBounds, this->pixmapCursor, rcBounds);
	////tmp.end();


	//HBITMAP hBitmap = QtWin::toHBITMAP(this->pixmapCursor, QtWin::HBitmapNoAlpha);

	//BITMAP Bmp = { 0 };
	//GetObject(hBitmap, sizeof(Bmp), &Bmp);

	//HDC hdcScreen = GetDC((HWND)this->winId());
	//HDC hdcMem = CreateCompatibleDC(NULL);
	//HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

	//auto bResult = StretchBlt(hdcScreen, 0, 0, 64, 64, hdcMem, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);


	//SelectObject(hdcMem, hbmOld);
	//DeleteDC(hdcMem);
	//ReleaseDC(NULL, hdcScreen);
}


//
//QPixmap MainWindow::GetCursorInfo() {
//	//SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
//	//SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
//
//
//	// Get information about the global cursor.
//	CURSORINFO ci;
//	ci.cbSize = sizeof(ci);
//	::GetCursorInfo(&ci);
//
//	//auto logicalDpiX = this->logicalDpiX();
//	//auto logicalDpiY = this->logicalDpiY();
//	//auto physicalDpiX = this->physicalDpiX();
//	//auto physicalDpiY = this->physicalDpiY();
//	auto dpi = devicePixelRatio();
//	auto qtDpi = devicePixelRatioF();
//	float dtDpi = qtDpi / dpiScaleFactorSystem;
//
//	// Get Cursor Size.
//	//int cursorWidth = GetSystemMetrics(SM_CXCURSOR) * dtDpi;
//	//int cursorHeight = GetSystemMetrics(SM_CYCURSOR) * dtDpi;
//	int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
//	int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
//
//	// Get your device contexts.
//	//HDC hdcScreen = GetDC(NULL);
//	HDC hdcScreen = GetDC((HWND)this->winId());
//	//HDC hdcMem = CreateCompatibleDC(hdcScreen);	
//	HDC hdcMem = CreateCompatibleDC(NULL);
//	
//
//	//// Draw the cursor into the canvas.
//	//DrawIcon(hdcMem, 0, 0, ci.hCursor);
//	////DrawIconEx(hdcMem, 0, 0, ci.hCursor, 256, 256, 0, nullptr, DI_NORMAL);
//
//	auto ii = CopyIcon(ci.hCursor);
//	ICONINFO iconinfo;
//	if (GetIconInfo(ii, &iconinfo)) {
//		HGDIOBJ hbmOld = SelectObject(hdcMem, iconinfo.hbmColor);
//
//		BITMAP Bmp = { 0 };
//		GetObject(iconinfo.hbmColor, sizeof(Bmp), &Bmp);
//
//
//		//DrawBitmap()
//
//
//		//HBITMAP hBmpCopy = (HBITMAP)CopyImage(iconinfo.hbmColor, IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
//
//		HDC hdcDest = CreateCompatibleDC(hdcScreen);
//		HBITMAP newHBitmap = CreateCompatibleBitmap(hdcScreen, Bmp.bmWidth, Bmp.bmHeight);
//		HGDIOBJ hmbOld_2 = SelectObject(hdcDest, newHBitmap);
//
//		//HBITMAP newHBitmap = ResizeBitmap(NULL, iconinfo.hbmColor);
//
//		auto bResult = StretchBlt(hdcScreen, 0, 0, 64, 64, hdcMem, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
//		//auto bResult = StretchBlt(hdcDest, 0, 0, 24, 24, hdcMem, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
//
//		//HBitmapToPng(iconinfo.hbmColor);
//
//		qDebug() << "hotXY = [" << iconinfo.xHotspot << ";" << iconinfo.yHotspot << "]";
//
//		// Convert to QPixmap.
//		//QPixmap cursorPixmap = QtWin::fromHBITMAP(iconinfo.hbmColor, QtWin::HBitmapNoAlpha);
//		QPixmap cursorPixmap = QtWin::fromHBITMAP(newHBitmap, QtWin::HBitmapNoAlpha);
//		//QPixmap cursorPixmap = QtWin::fromHBITMAP(hBmpCopy, QtWin::HBitmapNoAlpha);
//		
//		
//		this->pixmapCursor = cursorPixmap;
//		
//
//		SelectObject(hdcDest, hmbOld_2);
//		DeleteObject(newHBitmap);
//		DeleteDC(hdcDest);
//
//		SelectObject(hdcMem, hbmOld);
//	}
//
//
//	// Clean up after yourself.
//	DeleteDC(hdcMem);
//	ReleaseDC(NULL, hdcScreen);
//	return {};
//}



//QPixmap MainWindow::GetCursorInfo() {
//	//SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
//	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
//
//
//	// Get information about the global cursor.
//	CURSORINFO ci;
//	ci.cbSize = sizeof(ci);
//	::GetCursorInfo(&ci);
//
//	//auto logicalDpiX = this->logicalDpiX();
//	//auto logicalDpiY = this->logicalDpiY();
//	//auto physicalDpiX = this->physicalDpiX();
//	//auto physicalDpiY = this->physicalDpiY();
//	auto dpi = devicePixelRatio();
//	auto qtDpi = devicePixelRatioF();
//	float dtDpi = qtDpi / dpiScaleFactorSystem;
//
//	// Get Cursor Size.
//	//int cursorWidth = GetSystemMetrics(SM_CXCURSOR) * dtDpi;
//	//int cursorHeight = GetSystemMetrics(SM_CYCURSOR) * dtDpi;
//	int cursorWidth = GetSystemMetrics(SM_CXCURSOR) + 200;
//	int cursorHeight = GetSystemMetrics(SM_CYCURSOR) + 200;
//
//
//	int cx = GetSystemMetrics(SM_CXSMICON);
//	int cy = GetSystemMetrics(SM_CXSMICON);
//
//
//	//int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
//	//int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
//
//
//	// Get your device contexts.
//	HDC hdcScreen = GetDC(NULL);
//	HDC hdcMem = CreateCompatibleDC(hdcScreen);
//
//	// Create the bitmap to use as a canvas.
//	HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, cursorWidth, cursorHeight);
//
//	// Select the bitmap into the device context.
//	HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);
//
//
//	// Draw the cursor into the canvas.
//	DrawIcon(hdcMem, 0, 0, ci.hCursor);
//	//DrawIconEx(hdcMem, 0, 0, ci.hCursor, 256, 256, 0, nullptr, DI_NORMAL);
//
//	BITMAP Bmp = { 0 };
//	GetObject(hbmCanvas, sizeof(Bmp), &Bmp);
//
//	HBitmapToPng(hbmCanvas);
//
//
//	// Convert to QPixmap.
//	//QPixmap cursorPixmap = QtWin::fromHBITMAP(hbmCanvas, QtWin::HBitmapNoAlpha);
//	QPixmap cursorPixmap = QtWin::fromHBITMAP(hbmCanvas, QtWin::HBitmapNoAlpha);
//
//
//
//	//g_cursorInfo = CursorInfo{ reinterpret_cast<int>(ci.hCursor), PixmapToVectorBytes(cursorPixmap) };
//	////return CursorInfo{ reinterpret_cast<int>(ci.hCursor), PixmapToVectorBytes(cursorPixmap) };
//
//	//return g_cursorInfo;
//	//return cursorPixmap;
//
//	auto image = PixmapToVectorBytes(cursorPixmap);
//	auto pixmap = PixmapFromVectorBytes(image);
//
//	auto w = pixmap.width();
//	auto h = pixmap.height();
//
//
//	//this->pixmapCursor = pixmap.scaled(float(cursorWidth) / dpi, float(cursorHeight) / dpi, Qt::AspectRatioMode::KeepAspectRatio);
//	//this->pixmapCursor = pixmap.scaled(cursorWidth / qtDpi, cursorHeight / qtDpi);
//	//this->pixmapCursor = pixmap.scaled(24, 24);
//	this->pixmapCursor = pixmap;
//
//
//
//	// Clean up after yourself.
//	SelectObject(hdcMem, hbmOld);
//	DeleteObject(hbmCanvas);
//	DeleteDC(hdcMem);
//	ReleaseDC(NULL, hdcScreen);
//	return {};
//}