#pragma once

#ifndef SLIP_DLL_H
#define SLIP_DLL_H

#define SLIP_DLL_EXPORT __declspec(dllexport)

/*
    modules[0] = lagModule;
    modules[1] = dropModule;
    modules[2] = throttleModule;
    modules[3] = duplicatorModule;
    modules[4] = shuffleModule;
    modules[5] = encryptModule;
    modules[6] = trafficShaperModule;
*/
enum class SimulationType
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
    float delay;   // Only used for lag and throttle module
    float num;    // Only used for duplicate (numCopies) and shuffle (minPackets) modules
    bool drop;   // Only used for the throttle module
    SimulationType type;
};

/*
*   It is possible to pass a C# List as an argument to
*   a C++ function that expects a vector. You can do this by
*   using the array<Type>^ type in C#, which represents 
*   a managed array in the .NET Framework. When you pass an
*   array<Type>^ to a C++ function, it will be treated as a
*   vector of the corresponding type.
*/
extern "C"
{
    SLIP_DLL_EXPORT void startProgram(char* filterText, Config* configs, int configsCount);
    SLIP_DLL_EXPORT void stopProgram();
}

#endif // !SLIP_DLL_H
