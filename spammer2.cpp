#include <windows.h>
#include <string>

int main() {
    // Ask for confirmation
    int confirm = MessageBoxW(
        NULL,
        L"This will spam 'project' message boxes.\n\nRun spam? [Use Task Manager to close if needed] 🎗",
        L"Warning",
        MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2
    );

    if (confirm == IDNO) {
        MessageBoxW(NULL, L"Operation cancelled.", L"Info", MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    // Spam execution
    int count = 0;
    while (count < 100) {  // Safety limit
        MessageBoxW(
            NULL,
            L"project",
            L"Message",
            MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
        );
        count++;
        Sleep(100);  // Small delay
    }

    // Post-execution opinion
    MessageBoxW(
        NULL,
        L"Spam completed!\n\nWas this annoying? 😈\n"
        L"(Remember: Use responsibly!)",
        L"Final Message",
        MB_OK | MB_ICONINFORMATION
    );

    return 0;
}