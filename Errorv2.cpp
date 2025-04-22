#include <windows.h>
#include <math.h>
#include <ctime>

// Global variables
HWND hWnd;
HDC hdcScreen;
HBITMAP hBitmap;
HDC hdcMem;
int screenWidth, screenHeight;
double angle = 0.0;

// Function to rotate a point around a center
void RotatePoint(POINT& pt, POINT center, double angle) {
    double s = sin(angle);
    double c = cos(angle);

    // Translate point back to origin
    pt.x -= center.x;
    pt.y -= center.y;

    // Rotate point
    double xnew = pt.x * c - pt.y * s;
    double ynew = pt.x * s + pt.y * c;

    // Translate point back
    pt.x = static_cast<int>(xnew + center.x);
    pt.y = static_cast<int>(ynew + center.y);
}

// Function to draw rotated screen
void DrawRotatedScreen(HDC hdc) {
    // Get screen dimensions
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Create a memory DC to hold the screen capture
    hdcScreen = GetDC(NULL);
    hdcMem = CreateCompatibleDC(hdcScreen);
    hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(hdcMem, hBitmap);
    
    // Copy screen to memory DC
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);
    
    // Define the quad for the rotated screen
    POINT pt[4];
    pt[0].x = 0; pt[0].y = 0;
    pt[1].x = screenWidth; pt[1].y = 0;
    pt[2].x = screenWidth; pt[2].y = screenHeight;
    pt[3].x = 0; pt[3].y = screenHeight;
    
    // Center point for rotation
    POINT center;
    center.x = screenWidth / 2;
    center.y = screenHeight / 2;
    
    // Rotate all points
    for (int i = 0; i < 4; i++) {
        RotatePoint(pt[i], center, angle);
    }
    
    // Draw the rotated screen
    PlgBlt(hdc, pt, hdcMem, 0, 0, screenWidth, screenHeight, NULL, 0, 0);
    
    // Clean up
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static bool showedMessage = false;
    
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            // Draw rotated screen
            DrawRotatedScreen(hdc);
            
            // Show message box once when angle reaches certain point
            if (!showedMessage && angle > 1.5) { // ~90 degrees in radians
                showedMessage = true;
                MessageBox(hWnd, "Message", "lol", MB_OK);
            }
            
            EndPaint(hWnd, &ps);
            break;
        }
        
        case WM_TIMER: {
            // Update rotation angle
            angle += 0.05;
            if (angle > 6.28318) angle = 0; // 2*PI
            
            // Force redraw
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GDI Spin Class";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create window
    hWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        "GDI Spin Class",
        "GDI Screen Spin Effect",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL);
    
    if (!hWnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Make window transparent for the effect
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_COLORKEY);
    
    // Show window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    // Set timer for animation
    SetTimer(hWnd, 1, 16, NULL); // ~60fps
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    KillTimer(hWnd, 1);
    return (int) msg.wParam;
}