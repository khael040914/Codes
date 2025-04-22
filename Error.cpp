#include <windows.h>

// Manual function imports to avoid linker dependencies
typedef BOOL(WINAPI* BitBlt_t)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef BOOL(WINAPI* GetCursorPos_t)(LPPOINT);

int main() {
    // Load functions dynamically
    BitBlt_t myBitBlt = (BitBlt_t)GetProcAddress(LoadLibraryA("gdi32.dll"), "BitBlt");
    GetCursorPos_t myGetCursorPos = (GetCursorPos_t)GetProcAddress(LoadLibraryA("user32.dll"), "GetCursorPos");

    while (1) {
        HDC hdc = GetDC(0); // Get screen DC
        POINT cursor;
        myGetCursorPos(&cursor); // Get cursor position

        // Spam glitched BitBlt around cursor
        for (int i = 0; i < 10; i++) {
            int x = cursor.x + (rand() % 100 - 50); // Random offset X
            int y = cursor.y + (rand() % 100 - 50); // Random offset Y
            myBitBlt(hdc, x, y, 200, 200, hdc, x + (rand() % 20 - 10), y + (rand() % 20 - 10), SRCINVERT);
        }

        Sleep(50); // Adjust speed (lower = faster corruption)
        ReleaseDC(0, hdc);
    }
}