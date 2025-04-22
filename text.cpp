#include <windows.h>

// Define function pointer types
typedef BOOL(WINAPI* TextOut_t)(HDC, int, int, LPCSTR, int);
typedef COLORREF(WINAPI* SetTextColor_t)(HDC, COLORREF);

int main() {
    // Get function pointers
    TextOut_t pTextOut = (TextOut_t)GetProcAddress(GetModuleHandle("gdi32.dll"), "TextOutA");
    SetTextColor_t pSetTextColor = (SetTextColor_t)GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextColor");
    
    if (!pTextOut || !pSetTextColor) return 1;

    HDC hdc = GetDC(0);
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        for (int i = 0; i < 5; i++) {
            pSetTextColor(hdc, RGB(rand()%256, rand()%256, rand()%256));
            pTextOut(hdc, rand()%w, rand()%h, "facilityer.exe", 14);
        }
        Sleep(1000);
    }

    ReleaseDC(0, hdc);
    return 0;
}