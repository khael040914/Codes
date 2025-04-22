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
}glitch stays on screen
        int glitchSize = 60;
        int x = max(0, min(p.x - glitchSize/2, w - glitchSize));
        int y = max(0, min(p.y - glitchSize/2, h - glitchSize));
        myBitBlt(hdc, x, y, glitchSize, glitchSize, hdc, 
                x + (rand()%16 - 8), y + (rand()%16 - 8), 0x660046);
        
        ReleaseDC(0, hdc);
        Sleep(30);
    }
}tmap_t myCreateBitmap = (CreateCompatibleBitmap_t)GetProcAddress(hGDI, "CreateCompatibleBitmap");
    CreateCompatibleDC_t myCreateDC = (CreateCompatibleDC_t)GetProcAddress(hGDI, "CreateCompatibleDC");
    SelectObject_t mySelectObject = (SelectObject_t)GetProcAddress(hGDI, "SelectObject");
    GetCursorPos_t myGetCursorPos = (GetCursorPos_t)GetProcAddress(hUser32, "GetCursorPos");

    if(!myBitBlt || !myPatBlt || !myStretchBlt || !mySetPixel || 
       !myCreateBitmap || !myCreateDC || !mySelectObject || !myGetCursorPos) return 1;

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    // Stage 1: Full-screen glitches
    for (int i = 0; i < 300; i++) {
        HDC hdc = GetDC(0);
        int x1 = rand() % (w - 100);
        int y1 = rand() % (h - 100);
        myBitBlt(hdc, x1, y1, 100, 100, hdc, x1 + (rand()%40-20), y1 + (rand()%40-20), 0x660046);
        ReleaseDC(0, hdc);
        Sleep(20);
    }

    // Stage 2: Cursor glitches
    for (int i = 0; i < 150; i++) {
        HDC hdc = GetDC(0);
        POINT p;
        myGetCursorPos(&p);
        myPatBlt(hdc, 0, 0, w, h, PATINVERT);
        int x = max(0, min(p.x - 30, w - 60));
        int y = max(0, min(p.y - 30, h - 60));
        myBitBlt(hdc, x, y, 60, 60, hdc, x + (rand()%16-8), y + (rand()%16-8), SRCAND);
        ReleaseDC(0, hdc);
        Sleep(30);
    }

    // Stage 3: Optimized rainbow effect
    HDC hdcScreen = GetDC(0);
    HDC hdcMem = myCreateDC(hdcScreen);
    HBITMAP hbm = myCreateBitmap(hdcScreen, w, h);
    HBITMAP oldBmp = (HBITMAP)mySelectObject(hdcMem, hbm);
    
    float hue = 0;
    while (1) {
        // Generate rainbow pattern (optimized)
        for (int y = 0; y < h; y += 3) {
            for (int x = 0; x < w; x += 3) {
                mySetPixel(hdcMem, x, y, HSVtoRGB(fmod(hue + x*0.05f + y*0.05f, 1.0f, 1.0f));
            }
        }
        hue = fmod(hue + 1.5f, 360.0f);
        
        // Apply with glitch effects
        myStretchBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, w, h, SRCCOPY);
        myBitBlt(hdcScreen, rand()%50, rand()%50, 200, 200, hdcScreen, rand()%50, rand()%50, 0x008800C6);
        Sleep(33);  // ~30 FPS
    }
}