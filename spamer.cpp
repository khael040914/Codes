#include <windows.h>

int main() 
{
    int count = 0;
    const int maxSpam = 100; // Prevent infinite spam

    while (count < maxSpam) 
    {
        // Display MessageBox with error icon
        MessageBoxW(
            NULL,               // No parent window
            L"project",         // Message text
            L"Error",           // Title
            MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
        );
        
        count++;
        Sleep(100); // Small delay (100ms)
    }
    
    return 0;
}
// [This empty line is intentionally added to satisfy the newline requirement]