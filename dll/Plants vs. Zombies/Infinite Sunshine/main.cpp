#include "main.h"
#include <stdio.h>

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    int * base_addr;
    int * function_addr;
    int * sunshine_addr;
    char * message;

    int increase_step = 50;
    int time_span = 1000;

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:

            base_addr = (int *)0x00199180;
            function_addr = (int *)(*base_addr);
            sunshine_addr = function_addr + 5470;

            while(1){
                *sunshine_addr = (*sunshine_addr) + increase_step;
                Sleep(time_span);
            }

            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
