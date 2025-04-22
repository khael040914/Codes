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
typedef BOOL(WINAPI* PolyBezier_t)(HDC, const POINT*, DWORD);
typedef HPEN(WINAPI* CreatePen_t)(int, int, COLORREF);
typedef HGDIOBJ(WINAPI* SelectObject_t)(HDC, HGDIOBJ);
typedef BOOL(WINAPI* DeleteObject_t)(HGDIOBJ);

COLORREF Hue(int hue) {
    BYTE r, g, b;
    int segment = hue / 60;
    int remainder = hue % 60;
    int intensity = remainder * 255 / 60;

    switch (segment) {
        case 0: r = 255; g = intensity; b = 0; break;
        case 1: r = 255 - intensity; g = 255; b = 0; break;
        case 2: r = 0; g = 255; b = intensity; break;
        case 3: r = 0; g = 255 - intensity; b = 255; break;
        case 4: r = intensity; g = 0; b = 255; break;
        case 5: r = 255; g = 0; b = 255 - intensity; break;
        default: r = g = b = 0; break;
    }
    return RGB(r, g, b);
}

DWORD WINAPI cur(LPVOID lpParam) {
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

DWORD WINAPI beziers(LPVOID lpParam) {
    HINSTANCE hGDI = LoadLibraryA("gdi32.dll");
    GetSystemMetrics_t myGetSystemMetrics = (GetSystemMetrics_t)GetProcAddress(LoadLibraryA("user32.dll"), "GetSystemMetrics");
    GetDC_t myGetDC = (GetDC_t)GetProcAddress(LoadLibraryA("user32.dll"), "GetDC");
    ReleaseDC_t myReleaseDC = (ReleaseDC_t)GetProcAddress(LoadLibraryA("user32.dll"), "ReleaseDC");
    PolyBezier_t myPolyBezier = (PolyBezier_t)GetProcAddress(hGDI, "PolyBezier");
    CreatePen_t myCreatePen = (CreatePen_t)GetProcAddress(hGDI, "CreatePen");
    SelectObject_t mySelectObject = (SelectObject_t)GetProcAddress(hGDI, "SelectObject");
    DeleteObject_t myDeleteObject = (DeleteObject_t)GetProcAddress(hGDI, "DeleteObject");

    int sw = myGetSystemMetrics(SM_CXSCREEN);
    int sh = myGetSystemMetrics(SM_CYSCREEN);
    
    while (1) {
        HDC hdc = myGetDC(0);
        POINT p[4] = {
            {rand() % sw, rand() % sh},
            {rand() % sw, rand() % sh},
            {rand() % sw, rand() % sh},
            {rand() % sw, rand() % sh}
        };
        HPEN hPen = myCreatePen(PS_SOLID, 5, Hue(rand() % 240));
        mySelectObject(hdc, hPen);
        myPolyBezier(hdc, p, 4);
        myDeleteObject(hPen);
        myReleaseDC(0, hdc);
        Sleep(4);
    }
    FreeLibrary(hGDI);
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

    // Start all effect threads
    CreateThread(NULL, 0, cur, NULL, 0, NULL);
    CreateThread(NULL, 0, beziers, NULL, 0, NULL);

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

    // ===== STAGE 4: Spinning wave effects + Beziers =====
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

    FreeLibrary(hGDI);
    FreeLibrary(hUser32);
    return 0;
}d::max(0, std::min(static_cast<int>(p.y) - glitchSize/2, h - glitchSize));
            myBitBlt(hdc, x, y, glitchSize, glitchSize, hdc, 
                    x + (rand()%26 - 8), y + (rand()%19 - 8), SRCINVERT);
            
            myReleaseDC(0, hdc);
        }
        Sleep(30);
    }

    // ===== STAGE 3: Full-screen inversion =====
    for (int i = 0; i < 10 && audioRunning; i++) {
        HDC hdc = myGetDC(0);
        if (hdc) {
            myPatBlt(hdc, 0, 0, w, h, PATINVERT);
            myReleaseDC(0, hdc);
        }
        Sleep(100);
    }

    // ===== STAGE 4: Spinning wave effects =====
    double angle = 0.0;
    double wave_offset = 0.0;
    const int centerX = w/2;
    const int centerY = h/2;
    const int radius = std::min(w, h)/3;
    
    while (audioRunning) {
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

    // Cleanup
    audioRunning = false;
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    FreeLibrary(hGDI);
    FreeLibrary(hUser32);
    audio.join();
    
    return 0;
}
