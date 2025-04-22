#include <windows.h>
#include <iostream>

// Explicitly link with GDI32 (required for BitBlt)
#pragma comment(lib, "gdi32.lib")

int main()
{
    while (1) 
    {
        HDC hdcScreen = GetDC(NULL); // Get screen device context
        if (!hdcScreen) return 1;

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Perform BitBlt operation
        BitBlt(
            hdcScreen,          // Destination DC
            rand() % 2,        // Destination X
            rand() % 2,        // Destination Y
            screenWidth,        // Width
            screenHeight,      // Height
            hdcScreen,          // Source DC (same as destination)
            rand() % 2,        // Source X
            rand() % 2,        // Source Y
            SRCCOPY            // Raster operation
        );

        Sleep(10);
        ReleaseDC(NULL, hdcScreen);
    }
    return 0;
}
