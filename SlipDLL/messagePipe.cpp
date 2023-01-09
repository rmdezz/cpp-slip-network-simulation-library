#include <Windows.h>
#include "messagePipe.h"
#include <winnt.h>
#include <fileapi.h>
#include <handleapi.h>
#include <iostream>
#include <synchapi.h>
#include <iomanip>
#include <sstream>
#include <WinBase.h>

void sendMessage(const char* message)
{
    HANDLE pipe = CreateNamedPipe(L"\\\\.\\pipe\\messagePipe",
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        20,
        0,
        0,
        INFINITE,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        int error = (int)GetLastError();
        std::cerr << "Error code: " << error << std::endl;
        std::cerr << "Failed to create named pipe." << std::endl;
        return;
    }

    // Wait for a client to connect to the pipe
    bool connected = ConnectNamedPipe(pipe, NULL);

    if (!connected)
    {
        std::cerr << "Error connecting to named pipe" << std::endl;
        CloseHandle(pipe);
        return;
    }

    // Write the message to the named pipe

    time_t rawTime;
    struct tm timeInfo;
    char buffer[80];

    time(&rawTime);
    localtime_s(&timeInfo, &rawTime);

    // "%d-%m-%Y %I:%M:%S"

    /*
    *   %d: The day of the month as a decimal number (range 01 to 31).
        %b: The abbreviated month name according to the current locale.
        %a: The abbreviated day of the week name according to the current locale.
        %Y: The year with century as a decimal number.
        %T: The time in 24-hour notation (%H:%M:%S).
    */
    strftime(buffer, sizeof(buffer), "[%a %b %d %Y %T]", &timeInfo);

    std::stringstream messageStream;
    messageStream << buffer << std::setw(8) << " " << message;

    std::string finalMessage = messageStream.str();

    DWORD bytesWritten;
    BOOL success = WriteFile(pipe, finalMessage.c_str(), finalMessage.length(), &bytesWritten, NULL);

    if (!success)
    {
        int error = (int)GetLastError();
        std::cerr << "Error code: " << error << std::endl;
        std::cerr << "Error writing to the named pipe" << std::endl;    
        return;
    }

    success = FlushFileBuffers(pipe);

    if (!success)
    {
        std::cerr << "Error flushing the buffer" << std::endl;
        return;
    }

    // Disconnect the pipe, but do not close it
    DisconnectNamedPipe(pipe);

    // Close the pipe
    CloseHandle(pipe);
}

void sendMessageEx(std::stringstream& messageStream)
{
    std::string finalMessage = messageStream.str();
    sendMessage(finalMessage.c_str());
}