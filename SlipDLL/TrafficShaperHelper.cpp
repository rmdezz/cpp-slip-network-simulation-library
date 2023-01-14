#include "TrafficShaperHelper.h"
#include "NetSimulatorAPI.h"
#include "ModuleHelper.h"

void SetTrafficShaper(bool enabled, bool inbound, bool outbound, float chance, float speedLimit)
{
    Module* module = Divert::getTrafficShaperModule();
    if (!isModuleValid(module)) return;
    if (!isConfigValid(chance, speedLimit)) return;
    Config config = getConfig(enabled, inbound, outbound, chance, 0, speedLimit, false);
    configureModule(module, config);
}

void enableTrafficShaper()
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    enableModule(trafficShaperModule);
}

void disableTrafficShaper()
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    disableModule(trafficShaperModule);
}

bool isTrafficShaperEnabled()
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    return isModuleEnabled(trafficShaperModule);
}

void setTrafficShaperInbound(bool inbound)
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    return setInbound(trafficShaperModule, inbound);
}

bool getTrafficShaperInbound()
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    return getInbound(trafficShaperModule);
}

void setTrafficShaperOutbound(bool outbound)
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    return setOutbound(trafficShaperModule, outbound);
}

bool getTrafficShaperOutbound()
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    return getOutbound(trafficShaperModule);
}

void setTrafficShaperChance(float chance)
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    setChance(trafficShaperModule, chance);
}

bool getTrafficShaperChance()
{
    Module* trafficShaperModule = Divert::getTrafficShaperModule();
    return getChance(trafficShaperModule);
}

void setTrafficShaperSpeedLimit(float speedLimit)
{
    Module* trafficShapeModule = Divert::getShuffleModule();
    if (trafficShapeModule != nullptr)
    {
        showError();
        return;
    }
    trafficShapeModule->setNum(speedLimit);
}

float getTrafficShaperSpeedLimit()
{
    Module* trafficShapeModule = Divert::getShuffleModule();
    if (trafficShapeModule != nullptr)
    {
        showError();
        return 0;
    }
    return trafficShapeModule->getNum();
}
