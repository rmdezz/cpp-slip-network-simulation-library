// NetSimulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <time.h> 
#include "Divert.h"
#include "PacketList.h"
#include "GlobalVar.h"
#include <Windows.h>
#include <ctime>
#include <iomanip>
#include "datetime_formatter.h"

using namespace std;

struct Config
{
    bool enabled;
    bool inbound;
    bool outbound;
    double chance;
    double delay;      // Only used for the lag and throttle module
    int minPackets;   //  Only used for the shuffle module
    int numCopies;   //   Only used for the duplicate module
};

void disableStandardStreamSync();
void configureModules(Divert& divert, Config lagConfig, Config dropConfig, Config throttleConfig,
    Config encryptConfig, Config shuffleConfig, Config duplicateConfig);
void setModuleConfigs(Module* module, Config config);

//void startProgram(char* filterText,
//                  bool lagEnabled, bool lagIncoming, bool lagOutgoing, double lagDelay,
//                  bool dropEnabled, bool dropIncoming, bool dropOutgoing, double dropChance,
//                  bool throttleEnabled, bool throttleIncoming, bool throttleOutgoing, double throttleChance,
//                  bool encryptEnabled, bool encryptIncoming, bool encryptOutgoing, double encryptChance,
//                  bool shuffleEnabled, bool shuffleIncoming, bool shuffleOutgoing, double shuffleChance, int shuffleMinPackets,
//                  bool duplicateEnabled, bool duplicateIncoming, bool duplicateOutgoing, double duplicateChance, int duplicateNumCopies)
void startProgram(char* filterText, Config lagConfig, Config dropConfig, Config throttleConfig,
                  Config encryptConfig, Config shuffleConfig, Config duplicateConfig)
{
    // Set the output formatting with
    // two digits after the decimal point,
    // using fixed-point notation
    cout << fixed << setprecision(2);

    disableStandardStreamSync();
    
    // Create a packet list and a Divert object
    PacketList packetList;
    Divert divert(packetList);

    // Attempt to start Divert
    int start = divert.divertStart(filterText);

    // Check if Divert was successfully started
    if (!start)
    {
        // Print an error message with the current date and time
        cout << printDateTimeOS << "Failed to start. Try again later." << endl;
        return;
    }

    // Set up the modules
    configureModules(divert, lagConfig, dropConfig, throttleConfig,
        encryptConfig, shuffleConfig, duplicateConfig);
}

void disableStandardStreamSync()
{
    // Disable synchronization between the standard
    // C++ streams and the standard C streams
    // to improve performance
    std::ios_base::sync_with_stdio(false);
}

void configureModules(Divert& divert, Config lagConfig, Config dropConfig, Config throttleConfig,
                      Config encryptConfig, Config shuffleConfig, Config duplicateConfig)
{
    // Get modules
    Module* const* modules = divert.getModules();
    
    // Get configs
    Config configs[] = { lagConfig, dropConfig, throttleConfig, duplicateConfig, shuffleConfig, encryptConfig };

    for (int i = 0; divert.MODULE_COUNT; i++)
    {
        Config config = configs[i];

        // Toggle module
        modules[i]->setEnabledFlag(config.enabled);

        // Set configs
        setModuleConfigs(modules[i], config);
    }
}

void setModuleConfigs(Module* module, Config config)
{
    module->setInbound(config.inbound);
    module->setOutbound(config.outbound);
    module->setChance(config.chance);
    module->setTime(config.delay);
    module->setNum(config.numCopies);
}

void stopProgram()
{
    cout << printDateTimeOS << "Stopping..." << endl;
    GlobalVar::GetInstance()->stopLooping = true;
    cout << printDateTimeOS << "Program stopped successfully." << endl;
}


int main()
{
    cout << printDateTimeOS << "This is a test message" << endl;

    //GlobalVar::GetInstance()->printDateTime();

    cout << fixed << setprecision(2);

    PacketList packetList;
    Divert g_divert(packetList);

    std::ios_base::sync_with_stdio(false);

    // "ip.DstAddr == 140.82.114.3";
    // "inbound";
    const char* filterText = "ip.DstAddr == 140.82.114.3";
    int res = g_divert.divertStart(filterText);

    if (!res) return 0;

    Module* const* modules = g_divert.getModules();
    Module* firstModule = modules[0];

    //GlobalVar::GetInstance()->dropChance = 100;
    firstModule->setEnabledFlag(true);
    //GlobalVar::GetInstance()->lagTime = 500;
    

    while (GlobalVar::GetInstance()->stopLooping == false);

    return 0;
}
