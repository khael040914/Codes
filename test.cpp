#include <Windows.h>
#include <time.h>

// Ensure we link against gdi32.lib which contains BitBlt
#pragma comment(lib, "gdi32.lib")

int main() {
    // Initialize random seed
    srand((unsigned int)time(NULL));

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    
    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {  // Exit on ESC press
        HDC hdc = GetDC(NULL);
        if (!hdc) break;

        // Shift screen in all directions
        BitBlt(hdc, 0, 0, w, h, hdc, -30, 0, SRCCOPY);
        BitBlt(hdc, 0, 0, w, h, hdc, w - 30, 0, SRCCOPY);
        BitBlt(hdc, 0, 0, w, h, hdc, 0, -30, SRCCOPY);
        BitBlt(hdc, 0, 0, w, h, hdc, 0, h - 30, SRCCOPY);

        // Create and apply random color brush
        HBRUSH brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
        if (brush) {
            HGDIOBJ oldBrush = SelectObject(hdc, brush);
            BitBlt(hdc, 0, 0, w, h, hdc, 0, 0, PATINVERT);
            SelectObject(hdc, oldBrush);
            DeleteObject(brush);
        }

        ReleaseDC(NULL, hdc);
        Sleep(10);
    }
    FreeLibrary(hGdi32);
    FreeLibrary(hUser32);
    return 0;
}