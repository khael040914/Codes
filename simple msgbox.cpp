#include <windows.h>

// Proper function definition
void ShowMessageBox()
{
    MessageBoxW(NULL, L"Hello from function!", L"Function Example", MB_OK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ShowMessageBox(); // Call the function
    return 0;
}