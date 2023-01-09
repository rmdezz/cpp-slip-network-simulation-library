#include "Divert.h"
#include <iostream>

#include "GlobalVar.h"
#include "windivert.h"
#include <windows.h>
#include <iomanip>
#include <thread>
#include "messagePipe.h"
#include <sstream>

#define DIVERT_PRIORITY 0
#define QUEUE_LEN 2048
#define QUEUE_TIME 1024
#define CLOCK_WAIT_MS 40
#define MAX_PACKET_SIZE 0xFFFF

#define WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

using namespace std;

Module* Divert::lagModule = nullptr;
Module* Divert::modules[MODULE_COUNT] {nullptr};
Module* Divert::dropModule = nullptr;
Module* Divert::throttleModule = nullptr;
Module* Divert::duplicatorModule = nullptr;
Module* Divert::shuffleModule = nullptr;
Module* Divert::encryptModule = nullptr;
Module* Divert::trafficShaperModule = nullptr;


Divert::Divert(PacketList& packetList) : packetList(packetList)
{
    sendMessage("Starting divert...");

    lagModule = new LagModule(packetList);
    dropModule = new DropModule(packetList);
    throttleModule = new ThrottleModule(packetList);
    duplicatorModule = new DuplicationModule(packetList);
    shuffleModule = new ShuffleModule(packetList);
    encryptModule = new EncryptModule(packetList);
    trafficShaperModule = new TrafficShaperModule(packetList);
    
    modules[0] = lagModule;
    modules[1] = dropModule;
    modules[2] = throttleModule;
    modules[3] = duplicatorModule;
    modules[4] = shuffleModule;
    modules[5] = encryptModule;
    modules[6] = trafficShaperModule;

    t1_finished = t2_finished = true;
}

Divert::~Divert()
{
    while (t1_finished == false || t2_finished == false); // Wait for threads to finish

    sendMessage("Threads finished execution.");
    //cout << "Threads finished." << endl;

    if (lagModule != nullptr) delete lagModule;
    if (dropModule != nullptr) delete dropModule;
    if (throttleModule != nullptr) delete throttleModule;
    if (duplicatorModule != nullptr) delete duplicatorModule;
    if (shuffleModule != nullptr) delete shuffleModule;
    if (encryptModule != nullptr) delete encryptModule;
    if (trafficShaperModule != nullptr) delete trafficShaperModule;

    GlobalVar::DeleteInstance();

    //cout << "Divert destroyed.";
    sendMessage("Divert destroyed.");
}

void Divert::setConsoleTextColor(WORD color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), color);
}

/*
*   @brief Start the Divert simulation
*   @param filterText The text to use as a filter to specify which packets should be captured
*   by the WinDivert library.
*   @return An integer indicating the result of the function. A value of 0 indicates success,
*   while a non-zero value indicates an error.
*   @throw This function may throw an exception if there is an error opening the WinDivert 
*   handle or setting the WinDivert parameters.
*/
int Divert::divertStart(const char* filter)
{
    // Get a reference to the divert handle in the singleton object.
    HANDLE& divertHandle = GlobalVar::GetInstance()->divertHandle;

    // Open the WinDivert handle with the specified filter
    if (!openDivertHandle(filter, divertHandle)) return showDivertOpenError();

    sendMessage("WinDivert handle opened.");
    
    // Set the WinDivert parameters
    if (!setWinDivertParams(divertHandle)) return showSetParamsError();

    printWinDivertParams();

    resetModules();
    
    return startThreads();
}

int Divert::showError(int errorCode)
{
    std::stringstream messageStream;
    messageStream << "Error code: " << errorCode;
    sendMessageEx(messageStream);

    sendMessage("For more information about this error code, see: https://reqrypt.org/windivert-faq.html");

    return errorCode;
}

int Divert::showSetParamsError()
{
    int errorCode = (int)GetLastError();
    sendMessage("Error: WinDivert failed to set parameters.");
    return showError(errorCode);
}

int Divert::showDivertOpenError()
{
    int errorCode = (int)GetLastError();
    sendMessage("Error: WinDivert failed to open.");
    return showError(errorCode);
}

/*
*   @brief Open a WinDivert handle with the specified filter.
*   @param filterText The text to use as filter to specify which packets should be captured
    by the WinDivert library.
*   @param divertHandle A reference to a handle variable that will be set to the newly
    opened WinDivert handle.
*   @return A boolean value indicating whether the handle was successfully opened.
*/
bool Divert::openDivertHandle(const char* filter, HANDLE& divertHandle)
{
    // WINDIVERT_LAYER_NETWORK: Network packets to/from the local machine. This is the default layer.
    
    // WINDIVERT_PRIORITY_HIGHEST: This will ensure that packets are diverted to this handle before
    // they are diverted to any other handle with a lower priority level.
    divertHandle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, WINDIVERT_PRIORITY_HIGHEST, 0);
    return divertHandle != INVALID_HANDLE_VALUE;
}

bool Divert::setWinDivertParams(HANDLE& divertHandle)
{
    return WinDivertSetParam(divertHandle, WINDIVERT_PARAM_QUEUE_LENGTH, QUEUE_LEN)
        && WinDivertSetParam(divertHandle, WINDIVERT_PARAM_QUEUE_TIME, QUEUE_TIME);
}

void Divert::printWinDivertParams()
{
    std::stringstream messageStream;
    messageStream << left << setw(20) << "Packet queue length: " << right << QUEUE_LEN;
    std::string finalStream = messageStream.str();
    sendMessage(finalStream.c_str());

    messageStream.str(""); // Clears the content of the string-stream

    messageStream << left << setw(21) << "Packet queue time: " << right << QUEUE_TIME;
    finalStream = messageStream.str();
    sendMessage(finalStream.c_str());
}

void Divert::resetModules()
{
    for (int i = 0; i < MODULE_COUNT; i++)
    {
        modules[i]->setLastEnabled(false);
    }
}

/*
*   @brief Start the receivePackets and processPacketsPeriodically threads.
*   @return A boolean value indicating whether the threads were successfully created.
*   @throw This function may throw an exception if there is an error creating the threads.
*/
bool Divert::startThreads()
{
    sendMessage("Creating threads...");

    // Create the receivePackets and processPacketsPeriodically threads.
    receivePacketsThread = std::thread(&Divert::receivePackets, this);
    processPacketsThread = std::thread(&Divert::processPacketsPeriodically, this);

    // Check if the threads were successfully created.

    /*
        To check if a thread is not alive, you can call the std::thread::joinable()
        function on the thread object. This function returns true if the thread
        is joinable, meaning that it is still running or has not yet been joined.
        If the thread is not joinable, it returns false, which means that the
        thread has finished executing or has already been joined.
    */

    if (!receivePacketsThread.joinable() || !processPacketsThread.joinable())
    {
        // The threads have finished executing or have already been joined.
        sendMessage("Failed to create threads.");
        return false;
    }

    // Calling the detach() method on a std::thread object allows the thread
    // to continue executing in the background without blocking the main thread.
    receivePacketsThread.detach();
    processPacketsThread.detach();

    // Set the t1_finished and t2_finished variables to false.
    t1_finished = t2_finished = false;

    sendMessage("Threads created successfully.");
    GlobalVar::GetInstance()->stopLooping = false;

    return true;
}

bool Divert::createMutex()
{
    LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL; // no security attributes
    bool initialOwner = false; // initially not owned
    LPCTSTR lpName = NULL; // pointer to the name of the mutex
    HANDLE& mutex = GlobalVar::GetInstance()->mutex;
    mutex = CreateMutex(lpMutexAttributes, initialOwner, lpName);

    if (mutex == nullptr)
    {
        sendMessage("Error: Failed to create mutex.");
        return false;
    }
    return true;
}

bool Divert::createThread(LPTHREAD_START_ROUTINE lpStartAddress, Divert* pDivert, const char* threadName)
{
    LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr; // default security descriptor
    DWORD dwStackSize = 0;  // default stack size
    LPVOID lpParameter = pDivert; // // pass a pointer to the Divert object as the parameter // no parameter
    DWORD dwCreationFlags = 0; // run immediately
    LPDWORD lpThreadId = nullptr; // don't save thread ID
    HANDLE thread = CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);

    // check for errors
    if (thread == nullptr)
    {
        std::stringstream messageStream;
        messageStream << "Failed to create " << threadName << " thread.";
        std::string finalMessage = messageStream.str();
        sendMessage(finalMessage.c_str());
        return false;
    }

    return true;
}

/*
*   @brief Receive packets from the network and process them.
* 
*   This function receives packets from the network using the WinDivertRecv function
*   and process them using the processPacket function. If an error occurs while
*   receiving or processing a packet, the handleError function is called.
*   If the Divert handle is closed or the operation is aborted, the function exits.
*/
void Divert::receivePackets()
{
    // Allocate buffers for the packet data and address.
    char* packetBuffer = new char[MAX_PACKET_SIZE];
    WINDIVERT_ADDRESS* addressBuffer = new WINDIVERT_ADDRESS;
    UINT readLength;
    bool packetReceived;

    // Get a reference to the Divert handle.
    HANDLE& divertHandle = GlobalVar::GetInstance()->divertHandle;

    // Continuously receive and process packets.
    while (true)
    {
        // Receive a packet from the network. 
        // Reminder: WinDivertRecv is a blocking function.
        packetReceived = WinDivertRecv(divertHandle, packetBuffer, MAX_PACKET_SIZE, &readLength, addressBuffer);

        // If the packet could not be received, check if it was due to an error or
        // because the handle was closed or the operation was aborted.
        if (!packetReceived)
        {
            DWORD lastError = GetLastError();
            
            if (lastError == ERROR_INVALID_HANDLE || lastError == ERROR_OPERATION_ABORTED)
            {
                // If the handle was closed or the operation was aborted, exit the function.
                cout << "Receive packets thread finished." << endl;

                // Free the allocated memory
                delete[] packetBuffer;
                delete addressBuffer;

                t1_finished = true;
                return;
            }

            // If the error was not due to the handle being closed or the operation
            // being aborted, call the handleError() function.
            handleError();
            continue;
        }

        // Process the packet. If the function returns false, exit the loop.
        if (!processPacket(packetBuffer, readLength, addressBuffer))
        {
            // Free the allocated memory
            delete[] packetBuffer;
            delete addressBuffer;

            sendMessage("Receive packets thread finished.");
            t1_finished = true;
            break;
        }
    }
}

/*
*   @brief Process a single packet.
*   @param packetBuffer A buffer containing the packet data.
*   @param readLength The length of the packet data in the packetBuffer.
*   @param addressBuffer A buffer containing the packet address information.
*   @return A boolean value indicating whether the packet was successfully processed.
* 
*   This function processes a single packet by creating a Packet object with the provided
*   packet data and address information, acquiring a lock on the mutex, and calling the
*   processPacketIfNotStopped function. If the packet could not be processed, the function
*   returns false. 
*/
bool Divert::processPacket(char* packetBuffer, UINT readLength, WINDIVERT_ADDRESS* addressBuffer)
{
    sendMessage("Single captured packet matching the filter was received.");
    
    // Check if the packet was truncated during reception.
    if (isPacketTruncated(readLength, MAX_PACKET_SIZE))
    {
        sendMessage("Internal Error: WinDivertRecv truncated received packet.");
    }

    // Create a Packet object with the provided packet data and address information.
    Packet packet(packetBuffer, readLength, addressBuffer);

    // Acquire a lock on the mutex.
    // Reminder: Use lock_guard to acquire and release the mutex.
    std::unique_lock<std::timed_mutex> lock(mutex, std::defer_lock);

    lock.lock();

    // Process the packet
    if (!processPacketIfNotStopped(packet)) return false;

    // Release the lock on the mutex
    if (lock.owns_lock()) lock.unlock();

    return true;
}

bool Divert::isPacketTruncated(UINT readLength, UINT maxPacketSize)
{
    return readLength > MAX_PACKET_SIZE;
}

/*
*   @brief Process a packet if the program has not been stopped.
*   @param packet The Packet object to be processed.
*   @return True if the packet was sucessfully processed, false otherwise.
*   
*   This function checks if the program has been stopped by checking the stopLooping flag
*   in the GlobalVar singleton object. If the program has been stopped, it inserts the
*   packet into the packet list and processes it using the processModules function. If
*   the program has been stopped, it returns false.
*/
bool Divert::processPacketIfNotStopped(Packet& packet)
{
    if (GlobalVar::GetInstance()->stopLooping == true)
    {
        sendMessage("User stopped the program.");
        return false;
    }
        
    packetList.insertPacket(packet);
    processModules();
    return true;
}

/*
*   @brief Process all enabled modules.
*   
*   This function iterates through the list of modules and checks if each one is enabled.
*   If it is enabled, it starts the module using the startModule function. If it is not
*   enabled, it stops the module using the stopModule function. Finally, it sends the 
*   packets in the packet list to the network stack using the sendPackets function.
*/
void Divert::processModules()
{
    for (int i = 0; i < MODULE_COUNT; i++)
    {
        Module* module = modules[i];
        if (module->getEnabledFlag())
        {
            startModule(module);
        }
        else
        {
            stopModule(module);
        }
    }

    int packetsSent = packetList.sendPackets();
    
    //std::stringstream messageStream;
    //messageStream << "A total of " << packetsSent << " packets were sent to the network stack";
    //sendMessageEx(messageStream);
}

/*
*   @brief Start a module and process the packet if necessary.
*   @param A pointer to the module to be started and/or processed.
*   
*   This function starts the specified module if it is not already started, and then
*   processes the packet using the module's process function if necessary.
*   The module's processTriggered flag is updated based on whether the packet was processed.
*/
void Divert::startModule(Module*& module)
{
    // If the module is not already started, start it.
    if (!module->getLastEnabled())
    {
        std::stringstream messageStream;
        messageStream << "Initializing " << module->getShortName() << " module.";
        sendMessageEx(messageStream);

        module->startUp();
        module->setLastEnabled(true);
    }

    // Process the packet using the module's process function.
    if (module->process())  module->setProcessTriggered(true);
    else module->setProcessTriggered(false);
}

void Divert::stopModule(Module*& module)
{
    if (module->getLastEnabled())
    {
        module->closeDown();
        module->setLastEnabled(false);
    }
}

void Divert::handleError()
{
    int errorCode = (int)GetLastError();
    
    sendMessage("Error: WinDivert failed to read the packet");
    
    std::stringstream messageStream;
    messageStream << "Error code: " << errorCode;
    sendMessageEx(messageStream);

    sendMessage("For more information about this error code, see: https://reqrypt.org/windivert-doc.html#divert_recv");
}
    

/*
*   @brief Process packets at a fixed rate and send them to the network stack.
*   
*   This function processes packets in the packet list at a fixed rate by
*   acquiring a lock on the mutex, calling the processModules function to process
*   the packets, and then releasing the lock on the mutex. It then sends the
*   processed packets to the network stack. 
    
    If the lock on the mutex cannot be acquired within 40 miliseconds, the function
    waits for the fixed rate period to elapse before trying again. 
    If the stopLooping flag in the GlobalVar singleton is set, the function stops
    processing and sending packets and exits.
*/
void Divert::processPacketsPeriodically()
{
    DWORD startTime;

    while (true)
    {   
        startTime = GetTickCount64();
        std::unique_lock<std::timed_mutex> lock(mutex, std::defer_lock);
        
        // Try to acquire the lock on the mutex within 40 miliseconds.
        if (lock.try_lock_for(std::chrono::milliseconds(40)))
        {
            // Mutex was acquired within 40 miliseconds.

            // Process the packets and send them to the network stack.
            // Reminder: processModules function is the shared resource of mutex.
            processModules();

            // Release the lock on the mutex.
            if (lock.owns_lock()) lock.unlock();

            // Wait for the fixed rate period to elapse if necessary.
            SleepIfNotEnoughTimeElapsed(startTime, CLOCK_WAIT_MS);
        }
        else // Handle time-out.
        {
            // If the lock on the mutex could not be acquired within 40 milliseconds,
            // wait for the fixed rate period to elapse.
            Sleep(CLOCK_WAIT_MS);
        }

        // Check if the stopLooping flag is set.
        volatile bool& stopLooping = GlobalVar::GetInstance()->stopLooping;

        if (stopLooping)
        {
            // Stop modules. Send remaining packets to the network stack and exit.
            stopAndSendPackets();
            sendMessage("Process packets thread finished.");
            t2_finished = true;
            return;
        }
    }
}

/*
*   @brief Stop processing packets and send any remaining packets in the packet list
*   to the network stack.
* 
*   This function stops processing packets by calling the CloseDownAllModules function
*   and then sends any remaining packets in the packet list to the network stack by
*   calling the sendPackets function on the packetList object. It then closes the
*   WinDivert handle by calling the CloseDivertHandle function.
*/
void Divert::stopAndSendPackets()
{
    sendMessage("Stopping...");
    CloseDownAllModules();

    sendMessage("Sending remaining packets to the network stack...");
    int packets = packetList.sendPackets();
    
    std::stringstream messageStream;
    messageStream << "A total of " << packets << " packets were sent to the network stack.";
    sendMessageEx(messageStream);

    CloseDivertHandle();
}

void Divert::CloseDivertHandle()
{
    // WinDivertClose() closes the WinDivert handle and releases all associated resources.
    // BUT it does not update the value of the handle.
    WinDivertClose(GlobalVar::GetInstance()->divertHandle);

    // You will need to set the handle to INVALID_HANDLE_VALUE yourself.
    GlobalVar::GetInstance()->divertHandle = INVALID_HANDLE_VALUE;

    if (GlobalVar::GetInstance()->divertHandle == INVALID_HANDLE_VALUE)
        sendMessage("Divert handle closed.");
    else sendMessage("Failed to destroy Divert handle.");
}

void Divert::CloseDownAllModules()
{
    for (int i = 0; i < MODULE_COUNT; ++i)
    {
        Module* module = modules[i];

        if (module->getEnabledFlag())
        {
            module->closeDown();
        }
    }
}

void Divert::SleepIfNotEnoughTimeElapsed(DWORD startTick, DWORD minTime)
{
    DWORD elapsedTime = GetTickCount64() - startTick;

    if (elapsedTime < minTime)
    {
        Sleep(minTime - elapsedTime);
    }
}

Module* const* Divert::getModules() const
{
    return modules;
}

std::thread& Divert::getReceivePacketsThread()
{
    return receivePacketsThread;
}

std::thread& Divert::getProcessPacketsThread()
{
    return processPacketsThread;
}


