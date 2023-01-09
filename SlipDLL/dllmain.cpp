// dllmain.cpp : Defines the entry point for the DLL application.
//#include "pch.h"

#include <iostream>
#include <iomanip>

#include "Divert.h"
#include "PacketList.h"
#include "GlobalVar.h"

#include "datetime_manipulator.h"
#include "messagePipe.h"
#include "slipDLL.h"
#include "Helper.h"

/**
* @brief Start the network simulation program.
* @param filterText The filter to use as a filter for the packets.
* @param configs A list of Configs specifying the simulation settings for each feature.
*/
void startProgram(char* filterText, Config* configs, int configsCount)
{  
    // Create a packet list and a Divert object
    PacketList packetList;
    Divert divert(packetList);

    // Start Divert
    int start = divert.divertStart(filterText);

    // Check if Divert was successfully started
    if (!start)
    {
        sendMessage("Failed to start network simulator. Try again later.");
        return;
    }

    // Configure network simulator modules
    configureModules(divert, configs, configsCount);

    // Run simulation until stopped
    while (GlobalVar::GetInstance()->stopLooping == false);

    Sleep(1000);

    return;
}

/*
*   @brief Stop the network simulation program.
*   This function sends a message indicating that the program
*   is stopping and sets a global variable that signals the
*   program to stop looping.
*/
void stopProgram()
{
    sendMessage("Stopping the program.");
    GlobalVar::GetInstance()->stopLooping = true;
}