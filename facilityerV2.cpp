#include <windows.h>
#include <algorithm>
#include <cmath>

// Function pointer typedefs
typedef BOOL(WINAPI* BitBlt_t)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef BOOL(WINAPI* GetCursorPos_t)(LPPOINT);
typedef BOOL(WINAPI* PatBlt_t)(HDC, int, int, int, int, DWORD);
typedef int(WINAPI* GetSystemMetrics_t)(int);
typedef HDC(WINAPI* GetDC_t)(HWND);
typedef int(WINAPI* ReleaseDC_t)(HWND, HDC);
typedef BOOL(WINAPI* SetCursorPos_t)(int, int);
typedef BOOL(WINAPI* DrawIcon_t)(HDC, int, int, HICON);
typedef HICON(WINAPI* LoadIcon_t)(HINSTANCE, LPCSTR);

DWORD WINAPI cur(LPVOID lpParam) {
    // Get required functions
    HINSTANCE hUser32 = LoadLibraryA("user32.dll");
    GetCursorPos_t myGetCursorPos = (GetCursorPos_t)GetProcAddress(hUser32, "GetCursorPos");
    SetCursorPos_t mySetCursorPos = (SetCursorPos_t)GetProcAddress(hUser32, "SetCursorPos");
    DrawIcon_t myDrawIcon = (DrawIcon_t)GetProcAddress(hUser32, "DrawIcon");
    LoadIcon_t myLoadIcon = (LoadIcon_t)GetProcAddress(hUser32, "LoadIconA");
    GetSystemMetrics_t myGetSystemMetrics = (GetSystemMetrics_t)GetProcAddress(hUser32, "GetSystemMetrics");

    POINT cursor;
    int icon_x = myGetSystemMetrics(SM_CXICON);
    int icon_y = myGetSystemMetrics(SM_CYICON);
    
    while (1) {
        HDC hdc = GetDC(HWND_DESKTOP);
        myGetCursorPos(&cursor);
        int X = cursor.x + rand() % 3 - 1;
        int Y = cursor.y + rand() % 3 - 1;
        mySetCursorPos(X, Y);
        myDrawIcon(hdc, cursor.x - icon_x, cursor.y - icon_y, myLoadIcon(NULL, IDI_ERROR));
        ReleaseDC(0, hdc);
        Sleep(10);
    }
    FreeLibrary(hUser32);
    return 0;
}

int main() {
    // Load libraries
    HINSTANCE hGDI = LoadLibraryA("gdi32.dll");
    HINSTANCE hUser32 = LoadLibraryA("user32.dll");
    
    if (!hGDI || !hUser32) return 1;

    // Get function pointers
    BitBlt_t myBitBlt = (BitBlt_t)GetProcAddress(hGDI, "BitBlt");
    PatBlt_t myPatBlt = (PatBlt_t)GetProcAddress(hGDI, "PatBlt");
    GetCursorPos_t myGetCursorPos = (GetCursorPos_t)GetProcAddress(hUser32, "GetCursorPos");
    GetSystemMetrics_t myGetSystemMetrics = (GetSystemMetrics_t)GetProcAddress(hUser32, "GetSystemMetrics");
    GetDC_t myGetDC = (GetDC_t)GetProcAddress(hUser32, "GetDC");
    ReleaseDC_t myReleaseDC = (ReleaseDC_t)GetProcAddress(hUser32, "ReleaseDC");

    if (!myBitBlt || !myPatBlt || !myGetCursorPos || !myGetSystemMetrics || !myGetDC || !myReleaseDC) {
        FreeLibrary(hGDI);
        FreeLibrary(hUser32);
        return 1;
    }

    int w = myGetSystemMetrics(SM_CXSCREEN);
    int h = myGetSystemMetrics(SM_CYSCREEN);

    // Start cursor thread
    CreateThread(NULL, 0, cur, NULL, 0, NULL);

    // ===== STAGE 1: Full-screen glitches =====
    for (int i = 0; i < 300; i++) {
        HDC hdc = myGetDC(0);
        if (hdc) {
            int x1 = rand() % (w - 100);
            int y1 = rand() % (h - 100);
            int x2 = rand() % (w - 100);
            int y2 = rand() % (h - 100);
            myBitBlt(hdc, x1, y1, 100, 100, hdc, x2, y2, SRCINVERT);
            myReleaseDC(0, hdc);
        }
        Sleep(30);
    }

    // ===== STAGE 2: Cursor tracking glitches =====
    for (int i = 0; i < 300; i++) {
        HDC hdc = myGetDC(0);
        if (hdc) {
            POINT p;
            myGetCursorPos(&p);
            
            int glitchSize = 120;
            int x = std::max(0, std::min(static_cast<int>(p.x) - glitchSize/2, w - glitchSize));
            int y = std::max(0, std::min(static_cast<int>(p.y) - glitchSize/2, h - glitchSize));
            myBitBlt(hdc, x, y, glitchSize, glitchSize, hdc, 
                    x + (rand()%26 - 8), y + (rand()%19 - 8), SRCINVERT);
            
            myReleaseDC(0, hdc);
        }
        Sleep(30);
    }

    // ===== STAGE 3: Full-screen inversion =====
    for (int i = 0; i < 10; i++) {
        HDC hdc = myGetDC(0);
        if (hdc) {
            myPatBlt(hdc, 0, 0, w, h, PATINVERT);
            myReleaseDC(0, hdc);
        }
        Sleep(100);
    }

    // ===== STAGE 4: Spinning wave effects =====
    Sleep(1000);
    
    double angle = 0.0;
    double wave_offset = 0.0;
    const int centerX = w/2;
    const int centerY = h/2;
    const int radius = std::min(w, h)/3;
    
    while (true) {
        HDC hdc = myGetDC(0);
        if (hdc) {
            // Spinning effect
            angle += 0.1;
            for (int i = 0; i < 10; i++) {
                int x = centerX + static_cast<int>(radius * cos(angle + i * 0.2));
                int y = centerY + static_cast<int>(radius * sin(angle + i * 0.2));
                myBitBlt(hdc, x, y, 100, 100, hdc, 
                        x + (rand()%20 - 10), y + (rand()%20 - 10), SRCINVERT);
            }
            
            // Wave effect
            wave_offset += 0.05;
            for (int x = 0; x < w; x += 20) {
                int y_offset = static_cast<int>(30 * sin(wave_offset + x * 0.02));
                myBitBlt(hdc, x, h/2 + y_offset, 20, 20, hdc, 
                        x + 10, h/2 + y_offset + 10, SRCINVERT);
            }
            
            myReleaseDC(0, hdc);
        }
        Sleep(50);
    }

    // Cleanup (unreachable due to infinite loop)
    FreeLibrary(hGDI);
    FreeLibrary(hUser32);
    return 0;
}
