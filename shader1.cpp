#include <windows.h>

typedef BOOL(WINAPI* BitBlt_t)(HDC,int,int,int,int,HDC,int,int,DWORD);
typedef BOOL(WINAPI* PatBlt_t)(HDC,int,int,int,int,DWORD);
typedef BOOL(WINAPI* StretchBlt_t)(HDC,int,int,int,int,HDC,int,int,int,int,DWORD);
typedef BOOL(WINAPI* GetCursorPos_t)(LPPOINT);
typedef COLORREF(WINAPI* SetPixel_t)(HDC,int,int,COLORREF);
typedef HBITMAP(WINAPI* CreateCompatibleBitmap_t)(HDC,int,int);
typedef HDC(WINAPI* CreateCompatibleDC_t)(HDC);
typedef HGDIOBJ(WINAPI* SelectObject_t)(HDC,HGDIOBJ);
typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE Reserved;
	};
}_RGBQUAD, * PRGBQUAD; //credits to EthernalVortex
int red, green, blue;
bool ifcolorblue = false, ifblue = false;
COLORREF Hue(int length) { //credits to Void_/GetMBR again
	if (red != length) {
		red < length; red++;
		if (ifblue == true) {
			return RGB(red, 0, length);
		}
		else {
			return RGB(red, 0, 0);
		}
	}
	else {
		if (green != length) {
			green < length; green++;
			return RGB(length, green, 0);
		}
		else {
			if (blue != length) {
				blue < length; blue++;
				return RGB(0, length, blue);
			}
			else {
				red = 0; green = 0; blue = 0;
				ifblue = true;
			}
		}
	}
}
DWORD WINAPI shader3(LPVOID lpParam) { // salinewin shader lol
	  HINSTANCE hGDI = LoadLibraryA("gdi32.dll");
    GetSystemMetrics_t myGetSystemMetrics = (GetSystemMetrics_t)GetProcAddress(LoadLibraryA("user32.dll"), "GetSystemMetrics");
    GetDC_t myGetDC = (GetDC_t)GetProcAddress(LoadLibraryA("user32.dll"), "GetDC");
    ReleaseDC_t myReleaseDC = (ReleaseDC_t)GetProcAddress(LoadLibraryA("user32.dll"), "ReleaseDC");
    PolyBezier_t myPolyBezier = (PolyBezier_t)GetProcAddress(hGDI, "PolyBezier");
    CreatePen_t myCreatePen = (CreatePen_t)GetProcAddress(hGDI, "CreatePen");
    SelectObject_t mySelectObject = (SelectObject_t)GetProcAddress(hGDI, "SelectObject");
    DeleteObject_t myDeleteObject = (DeleteObject_t)GetProcAddress(hGDI, "DeleteObject");
   
   HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].rgb += x^y;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}