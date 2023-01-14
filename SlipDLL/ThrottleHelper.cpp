#include "ThrottleHelper.h"
#include "NetSimulatorAPI.h"
#include "ModuleHelper.h"

void SetThrottle(bool enabled, bool inbound, bool outbound, float chance, float delay, bool drop)
{
    Module* throttleModule = Divert::getThrottleModule();
    if (!isModuleValid(throttleModule)) return;
    if (!isConfigValid(chance, delay)) return;
    Config throttleConfig = getConfig(enabled, inbound, outbound, chance, delay, 0, drop);
    configureModule(throttleModule, throttleConfig);
}

void enableThrottle()
{
    Module* throtleModule = Divert::getThrottleModule();
    enableModule(throtleModule);
}

void disableThrottle()
{
    Module* throttleModule = Divert::getThrottleModule();
    disableModule(throttleModule);
}

bool isThrottleEnabled()
{
    Module* throttleModule = Divert::getThrottleModule();
    return isModuleEnabled(throttleModule);
}

void setThrottleInbound(bool inbound)
{
    Module* throttleModule = Divert::getThrottleModule();
    return setInbound(throttleModule, inbound);
}

bool getThrottleInbound()
{
    Module* throttleModule = Divert::getThrottleModule();
    return getInbound(throttleModule);
}

void setThrottleOutbound(bool outbound)
{
    Module* throttleModule = Divert::getThrottleModule();
    return setOutbound(throttleModule, outbound);
}

bool getThrottleOutbound()
{
    Module* throttleModule = Divert::getThrottleModule();
    return getOutbound(throttleModule);
}

void setThrottleChance(float chance)
{
    Module* throttleModule = Divert::getThrottleModule();
    setChance(throttleModule, chance);
}

bool getThrottleChance()
{
    Module* throttleModule = Divert::getThrottleModule();
    return getChance(throttleModule);
}

void setThrottleTime(float time)
{
    Module* throttleModule = Divert::getThrottleModule();
    setDelay(throttleModule, time);
}

float getThrottleTime()
{
    Module* throttleModule = Divert::getThrottleModule();
    return getDelay(throttleModule);
}

void setThrottleDrop(bool drop)
{
    Module* dropModule = Divert::getDropModule();
    if (dropModule == nullptr)
    {
        showError();
        return;
    }
    dropModule->setDrop(drop);
}

bool getThrottleDrop()
{
    Module* throttleModule = Divert::getThrottleModule();
    if (throttleModule == nullptr)
    {
        showError();
        return false;
    }
    return throttleModule->getDrop();
}