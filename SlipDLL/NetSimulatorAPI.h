#pragma once

#ifndef SLIP_DLL_H
#define SLIP_DLL_H

#include <Windows.h>

#define EXPORT __declspec(dllexport)

/*
*   @brief It will ensure that the C++ name mangling does not occur on the enum,
    structs and functions, and allow the C code to use them with the same
    naming conventions.
*/
extern "C"
{
    enum SimulationType
    {
        Lag,
        Drop,
        Throttle,
        Duplicate,
        Shuffle,
        Encrypt,
        TrafficShaper
    };

#pragma pack(1)
    struct Config
    {
        bool enabled;
        bool inbound;
        bool outbound;
        float chance;
        float delay;   // Only used for lag (delay) and throttle (time) modules
        float num;    // Only used for duplicate (numCopies), shuffle (minPackets), and traffic shaper (speedLimit) modules
        bool drop;   // Only used for the throttle module
        SimulationType type;
    };

    EXPORT void startSimulationEx(const char* filterText, Config* configs, int configsCount, HANDLE eventHandle);
    EXPORT void startSimulation(const char* filterText, HANDLE eventHandle);
    EXPORT void stopSimulation();

    EXPORT void SetLag(bool enabled, bool inbound, bool outbound, float delay);
    EXPORT void enableLag();
    EXPORT void disableLag();
    EXPORT bool isLagEnabled();
    EXPORT void setLagDelay(float delay);
    EXPORT float getLagDelay();
    EXPORT void setLagInbound(bool inbound);
    EXPORT bool getLagInbound();
    EXPORT void setLagOutbound(bool outbound);
    EXPORT bool getLagOutbound();

    EXPORT void SetDrop(bool enabled, bool inbound, bool outbound, float chance);
    EXPORT void enableDrop();
    EXPORT void disableDrop();
    EXPORT bool isDropEnabled();
    EXPORT void setDropInbound(bool inbound);
    EXPORT bool getDropInbound();
    EXPORT void setDropOutbound(bool outbound);
    EXPORT bool getDropOutbound();
    EXPORT void setDropChance(float chance);
    EXPORT bool getDropChance();

    EXPORT void SetDuplicate(bool enabled, bool inbound, bool outbound, float chance, unsigned int numCopies);
    EXPORT void enableDuplicate();
    EXPORT void disableDuplicate();
    EXPORT bool isDuplicateEnabled();
    EXPORT void setDuplicateInbound(bool inbound);
    EXPORT bool getDuplicateInbound();
    EXPORT void setDuplicateOutbound(bool outbound);
    EXPORT bool getDuplicateOutbound();
    EXPORT void setDuplicateChance(float chance);
    EXPORT bool getDuplicateChance();
    EXPORT void setDuplicateCopies(unsigned int copies);
    EXPORT unsigned int getDuplicateCopies();

    EXPORT void SetShuffle(bool enabled, bool inbound, bool outbound, float chance, unsigned int minPackets);
    EXPORT void enableShuffle();
    EXPORT void disableShuffle();
    EXPORT bool isShuffleEnabled();
    EXPORT void setShuffleInbound(bool inbound);
    EXPORT bool getShuffleInbound();
    EXPORT void setShuffleOutbound(bool outbound);
    EXPORT bool getShuffleOutbound();
    EXPORT void setShuffleChance(float chance);
    EXPORT bool getShuffleChance();
    EXPORT void setShuffleMinPackets(unsigned int minPackets);
    EXPORT unsigned int getShuffleMinPackets();

    EXPORT void SetTrafficShaper(bool enabled, bool inbound, bool outbound, float chance, float speedLimit);
    EXPORT void enableTrafficShaper();
    EXPORT void disableTrafficShaper();
    EXPORT bool isTrafficShaperEnabled();
    EXPORT void setTrafficShaperInbound(bool inbound);
    EXPORT bool getTrafficShaperInbound();
    EXPORT void setTrafficShaperOutbound(bool outbound);
    EXPORT bool getTrafficShaperOutbound();
    EXPORT void setTrafficShaperChance(float chance);
    EXPORT bool getTrafficShaperChance();
    EXPORT void setTrafficShaperSpeedLimit(float speedLimit);
    EXPORT float getTrafficShaperSpeedLimit();

    EXPORT void SetEncrypt(bool enabled, bool inbound, bool outbound, float chance);
    EXPORT void enableEncrypt();
    EXPORT void disableEncrypt();
    EXPORT bool isEncryptEnabled();
    EXPORT void setEncryptInbound(bool inbound);
    EXPORT bool getEncryptInbound();
    EXPORT void setEncryptOutbound(bool outbound);
    EXPORT bool getEncryptOutbound();
    EXPORT void setEncryptChance(float chance);
    EXPORT bool getEncryptChance();

    EXPORT void SetThrottle(bool enabled, bool inbound, bool outbound, float chance, float delay, bool drop);
    EXPORT void enableThrottle();
    EXPORT void disableThrottle();
    EXPORT bool isThrottleEnabled();
    EXPORT void setThrottleInbound(bool inbound);
    EXPORT bool getThrottleInbound();
    EXPORT void setThrottleOutbound(bool outbound);
    EXPORT bool getThrottleOutbound();
    EXPORT  void setThrottleChance(float chance);
    EXPORT bool getThrottleChance();
    EXPORT void setThrottleTime(float time);
    EXPORT float getThrottleTime();
    EXPORT void setThrottleDrop(bool drop);
    EXPORT bool getThrottleDrop();

    EXPORT HANDLE createEvent();
    EXPORT void waitEvent(HANDLE eventHandle);

    EXPORT HANDLE startPipeClient();
    EXPORT void stopPipeClient(HANDLE processHandle);
}

#endif // !SLIP_DLL_H
