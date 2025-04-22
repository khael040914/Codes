#include <windows.h>
#include <ctime>

HCURSOR hErrorCursor;
POINT oldPos;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static POINT pts[20];
    static int count = 0;
    
    switch(msg) {
        case WM_CREATE:
            hErrorCursor = LoadCursor(NULL, IDC_HAND);
            SetTimer(hWnd, 1, 50, NULL);
            break;
            
        case WM_TIMER:
            GetCursorPos(&pts[count % 20]);
            count = (count + 1) % 20;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            for(int i = 0; i < min(count, 20); i++) {
                DrawIcon(hdc, pts[i].x, pts[i].y, hErrorCursor);
            }
            
            EndPaint(hWnd, &ps);
            break;
        }
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "ErrorSpam";
    RegisterClass(&wc);
    
    HWND hWnd = CreateWindow("ErrorSpam", "Error Spam", WS_POPUP | WS_VISIBLE,
                           0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                           NULL, NULL, hInst, NULL);
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}