#include <windows.h>

// Manual imports (no linker needed)
typedef BOOL(WINAPI* BitBlt_t)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef BOOL(WINAPI* GetCursorPos_t)(LPPOINT);

int main() {
    // Load functions dynamically
    BitBlt_t myBitBlt = (BitBlt_t)GetProcAddress(LoadLibraryA("gdi32.dll"), "BitBlt");
    GetCursorPos_t myGetCursorPos = (GetCursorPos_t)GetProcAddress(LoadLibraryA("user32.dll"), "GetCursorPos");

    while (1) {
        HDC hdc = GetDC(0); // Get screen DC
        int w = GetSystemMetrics(SM_CXSCREEN); // Screen width
        int h = GetSystemMetrics(SM_CYSCREEN); // Screen height

        // 1. FULL-SCREEN GLITCH (original BitBlt madness)
        myBitBlt(hdc, rand() % 50, rand() % 50, w, h, hdc, rand() % 50, rand() % 50, SRCINVERT);

        // 2. CURSOR-CENTERED CORRUPTION (Saline-style)
        POINT cursor;
        myGetCursorPos(&cursor);
        for (int i = 0; i < 5; i++) { // Spam glitches near cursor
            int x = cursor.x + (rand() % 100 - 50);
            int y = cursor.y + (rand() % 100 - 50);
            myBitBlt(hdc, x, y, 150, 150, hdc, x + (rand() % 40 - 20), y + (rand() % 40 - 20), SRCINVERT);
        }

        Sleep(30); // Faster = more chaos
        ReleaseDC(0, hdc);
    }
}