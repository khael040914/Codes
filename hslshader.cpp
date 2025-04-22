#include <windows.h>
#include <math.h>

// Function pointer types
typedef BOOL(WINAPI* DeleteObjectFunc)(HGDIOBJ);
typedef HDC(WINAPI* GetDCFunc)(HWND);
typedef int(WINAPI* ReleaseDCFunc)(HWND, HDC);
typedef HDC(WINAPI* CreateCompatibleDCFunc)(HDC);
typedef HBITMAP(WINAPI* CreateBitmapFunc)(int, int, UINT, UINT, const void*);
typedef HGDIOBJ(WINAPI* SelectObjectFunc)(HDC, HGDIOBJ);
typedef BOOL(WINAPI* BitBltFunc)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef LONG(WINAPI* SetBitmapBitsFunc)(HBITMAP, LONG, const void*);
typedef LONG(WINAPI* GetBitmapBitsFunc)(HBITMAP, LONG, LPVOID);
typedef int(WINAPI* GetSystemMetricsFunc)(int);

typedef union _RGBQUAD {
    COLORREF rgb;
    struct {
        BYTE b;
        BYTE g;
        BYTE r;
        BYTE unused;
    };
} *PRGBQUAD;

// Global state for color cycling
int red = 0, green = 0, blue = 0;
bool colorCycle = false;

COLORREF GenerateColor() {
    // Simple color cycling effect
    if (!colorCycle) {
        red = (red + 5) % 256;
        if (red == 0) {
            green = (green + 5) % 256;
            if (green == 0) {
                blue = (blue + 5) % 256;
                if (blue == 0) colorCycle = true;
            }
        }
    } else {
        red = (red - 5) % 256;
        if (red == 0) {
            green = (green - 5) % 256;
            if (green == 0) {
                blue = (blue - 5) % 256;
                if (blue == 0) colorCycle = false;
            }
        }
    }
    return RGB(red, green, blue);
}

DWORD WINAPI ShaderThread(LPVOID lpParam) {
    // Load GDI functions
    HMODULE hGDI32 = LoadLibrary("gdi32.dll");
    HMODULE hUser32 = LoadLibrary("user32.dll");
    
    if (!hGDI32 || !hUser32) return 1;

    GetDCFunc pGetDC = (GetDCFunc)GetProcAddress(hUser32, "GetDC");
    ReleaseDCFunc pReleaseDC = (ReleaseDCFunc)GetProcAddress(hUser32, "ReleaseDC");
    GetSystemMetricsFunc pGetSystemMetrics = (GetSystemMetricsFunc)GetProcAddress(hUser32, "GetSystemMetrics");
    
    CreateCompatibleDCFunc pCreateCompatibleDC = (CreateCompatibleDCFunc)GetProcAddress(hGDI32, "CreateCompatibleDC");
    CreateBitmapFunc pCreateBitmap = (CreateBitmapFunc)GetProcAddress(hGDI32, "CreateBitmap");
    SelectObjectFunc pSelectObject = (SelectObjectFunc)GetProcAddress(hGDI32, "SelectObject");
    BitBltFunc pBitBlt = (BitBltFunc)GetProcAddress(hGDI32, "BitBlt");
    GetBitmapBitsFunc pGetBitmapBits = (GetBitmapBitsFunc)GetProcAddress(hGDI32, "GetBitmapBits");
    SetBitmapBitsFunc pSetBitmapBits = (SetBitmapBitsFunc)GetProcAddress(hGDI32, "SetBitmapBits");
    DeleteObjectFunc pDeleteObject = (DeleteObjectFunc)GetProcAddress(hGDI32, "DeleteObject");

    if (!pGetDC || !pReleaseDC || !pGetSystemMetrics || !pCreateCompatibleDC || 
        !pCreateBitmap || !pSelectObject || !pBitBlt || !pGetBitmapBits || 
        !pSetBitmapBits || !pDeleteObject) {
        FreeLibrary(hGDI32);
        FreeLibrary(hUser32);
        return 1;
    }

    int width = pGetSystemMetrics(SM_CXSCREEN);
    int height = pGetSystemMetrics(SM_CYSCREEN);
    int size = width * height;
    
    _RGBQUAD* screenData = (_RGBQUAD*)VirtualAlloc(0, size * sizeof(_RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!screenData) {
        FreeLibrary(hGDI32);
        FreeLibrary(hUser32);
        return 1;
    }

    while (true) {
        HDC hdcScreen = pGetDC(NULL);
        HDC hdcMem = pCreateCompatibleDC(hdcScreen);
        HBITMAP hBitmap = pCreateBitmap(width, height, 1, 32, screenData);
        
        if (!hdcScreen || !hdcMem || !hBitmap) {
            if (hBitmap) pDeleteObject(hBitmap);
            if (hdcMem) pDeleteObject(hdcMem);
            if (hdcScreen) pReleaseDC(NULL, hdcScreen);
            continue;
        }

        pSelectObject(hdcMem, hBitmap);
        pBitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
        pGetBitmapBits(hBitmap, size * 4, screenData);

        // Apply effect to each pixel
        COLORREF color = GenerateColor();
        for (int i = 0; i < size; i++) {
            screenData[i].rgb = color;
        }

        pSetBitmapBits(hBitmap, size * 4, screenData);
        pBitBlt(hdcScreen, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

        // Clean up
        pDeleteObject(hBitmap);
        pDeleteObject(hdcMem);
        pReleaseDC(NULL, hdcScreen);

        Sleep(16); // ~60 FPS
    }

    VirtualFree(screenData, 0, MEM_RELEASE);
    FreeLibrary(hGDI32);
    FreeLibrary(hUser32);
    return 0;
}

int main() {
    // Create the shader thread
    HANDLE hThread = CreateThread(NULL, 0, ShaderThread, NULL, 0, NULL);
    if (!hThread) return 1;

    // Message loop to keep the application running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    TerminateThread(hThread, 0);
    CloseHandle(hThread);
    return 0;
}