#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <tlhelp32.h>

#define FALSE 0
#define TRUE 1

int inject(int pid, char* dllPath){
    HANDLE procHandler;

    LPVOID remoteMemory;
    LPVOID remoteLoadLibrary;
    LPVOID remoteKernel32;

    int size = strlen(dllPath);

    // 1. Open process
    procHandler = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if(procHandler == NULL){
        printf("[-] Open process error!\n");
        return FALSE;
    }
    printf("[+] Open process success!(%p)\n", procHandler);

    // 2. Allocate memory in the target process
    remoteMemory = VirtualAllocEx(procHandler, NULL, dllPath, MEM_COMMIT, PAGE_READWRITE);
    if(remoteMemory == NULL){
        printf("[-] Allocate memory error!\n");
        return FALSE;
    }
    printf("[+] Allocate memory success!(%p)\n", remoteMemory);

    // 3. Write dll path to remote process
    if(!WriteProcessMemory(procHandler, remoteMemory, dllPath, size, NULL)){
        printf("[-] Write memory error!\n");
        return FALSE;
    }
    printf("[+] Write memory success!\n");

    // 4. Get module handler of kernel32.dll
    remoteKernel32 = GetModuleHandleA("kernel32.dll");
    if(remoteKernel32 == NULL){
        printf("[-] Get handler of kernel32.dll error!\n");
        return FALSE;
    }
    printf("[+] Get handler of kernel32.dll success!(%p)\n", remoteKernel32);

    // 5. Get address of function LoadLibraryA
    remoteLoadLibrary = GetProcAddress(remoteKernel32, "LoadLibraryA");
    if(remoteLoadLibrary == NULL){
        printf("[-] Get handler of LoadLibraryA error!\n");
        return FALSE;
    }
    printf("[+] Get handler of LoadLibraryA success!(%p)\n", remoteLoadLibrary);

    // 6. Create remote thread
    if(!CreateRemoteThread(procHandler, NULL, 0, remoteLoadLibrary, remoteMemory, 0, NULL)){
        printf("[-] Create remote thread error!\n");
        return FALSE;
    }
    printf("[+] Create remote thread success!\n");
    return TRUE;

}


DWORD getPid(char* processName){
    DWORD pid = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, processName) == 0)
            {
                pid = entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return pid;
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        printf("Usage: \n");
        printf("\t%s [GAME_PROGRESS_NAME] [DLL_PATH]\n", argv[0]);
        exit(1);
    }
    int pid = getPid(argv[1]);
    if (pid == 0){
            printf("[-] You have no game running!\n");
            exit(2);
    }
    printf("[+] Detected game pid: %d\n", pid);
    inject(pid, argv[2]);
    return 0;
}
