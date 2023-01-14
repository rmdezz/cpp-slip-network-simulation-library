#include <Windows.h>
#include <stdio.h>
#include "NetSimulatorAPI.h"

HANDLE startPipeClient()
{
    // Get the current working directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exePath, '\\');
    if (lastSlash != NULL)
    {
        *lastSlash = '\0';
    }

    // Construct the path to SlipClient.exe
    wchar_t slipClientPath[MAX_PATH];
    wcscpy_s(slipClientPath, exePath);
    wcscat_s(slipClientPath, L"\\SlipClient.exe");

    // Check if SlipClient.exe exists in the current directory
    DWORD fileAttributes = GetFileAttributes(slipClientPath);
    if (fileAttributes == INVALID_FILE_ATTRIBUTES || (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        const char* message = "Error: SlipClient.exe not found in the current directory.";
        MessageBoxA(NULL,
            message,
            "Error",
            MB_ICONERROR | MB_OK);
        return NULL;
    }

    // Start SlipClient.exe as a separate process

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"open";
    sei.lpFile = slipClientPath;
    sei.nShow = SW_SHOWNORMAL;
    sei.fMask = SEE_MASK_NOCLOSEPROCESS; // Used to retrieve a handle to the process created by the shell.
    
    // Start
    bool result = ShellExecuteEx(&sei);
    if (!result)
    {
        DWORD errorCode = GetLastError();
        LPWSTR errorMessage = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);
        MessageBoxW(NULL, errorMessage, L"Error starting SlipClient.exe", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
    }

    return sei.hProcess;
}

void stopPipeClient(HANDLE processHandle)
{
    if (processHandle == NULL)
    {
        DWORD errorCode = GetLastError();
        LPWSTR errorMessage = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);
        MessageBoxW(NULL, errorMessage, L"Can't get the process handle", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
    }

    // Terminate the process
    if (!TerminateProcess(processHandle, 0))
    {
        // Handle error
        DWORD errorCode = GetLastError();
        LPWSTR errorMessage = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);
        MessageBoxW(NULL, errorMessage, L"Cannot terminate the pipe client process.", MB_ICONERROR | MB_OK);
        LocalFree(errorMessage);
    }

    // Close the process handle
    CloseHandle(processHandle);
}