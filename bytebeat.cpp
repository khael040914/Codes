#include <iostream>
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Simple audio output for Windows (using Beep) and Linux (using console beep)
void playSample(uint8_t sample) {
    // Scale the 8-bit sample to a frequency (100-2000Hz)
    int frequency = 100 + (sample * 1900 / 255);
    
    #ifdef _WIN32
    Beep(frequency, 10);
    #else
    std::cout << "\a"; // Console beep (limited functionality)
    usleep(1000); // Small delay
    #endif
}

int main() {
    uint32_t t = 0;
    const uint32_t sampleRate = 8000; // Samples per second
    
    std::cout << "Playing bytebeat... Press Ctrl+C to stop." << std::endl;
    
    while (true) {
        // The bytebeat formula
        uint8_t sample = (
            (t >> 10 | t | t >> 911) * 20 + ((t >> 11) & 7) >> 
            (t * ((t & 4096 ? t % 65536 < 59392 ? 7 : t & 9 : 16) ^ 
            (10 & t >> 14)) >> (3 & -t >> (t & 2048 ? 2 : 104))) >> 
            (t&t>>9) >> 192 >> 1 >> 2019
        ) & 0xFF;
        
        playSample(sample);
        
        t++;
        
        // Simple timing control
        #ifdef _WIN32
        Sleep(1000/sampleRate);
        #else
        usleep(1000000/sampleRate);
        #endif
    }
    
    return 0;
}