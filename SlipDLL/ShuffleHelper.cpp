#include "ShuffleHelper.h"
#include "NetSimulatorAPI.h"
#include "ModuleHelper.h"

void SetShuffle(bool enabled, bool inbound, bool outbound, float chance, unsigned int minPackets)
{
    Module* module = Divert::getShuffleModule();
    if (!isModuleValid(module)) return;
    if (!isConfigValid(chance, SimulationType::Shuffle)) return;
    Config config = getConfig(enabled, inbound, outbound, chance, 0, minPackets, false);
    configureModule(module, config);
}

void enableShuffle()
{
    Module* shuffleModule = Divert::getShuffleModule();
    enableModule(shuffleModule);
}

void disableShuffle()
{
    Module* shuffleModule = Divert::getShuffleModule();
    disableModule(shuffleModule);
}

bool isShuffleEnabled()
{
    Module* shuffleModule = Divert::getShuffleModule();
    return isModuleEnabled(shuffleModule);
}

void setShuffleInbound(bool inbound)
{
    Module* shuffleModule = Divert::getShuffleModule();
    return setInbound(shuffleModule, inbound);
}

bool getShuffleInbound()
{
    Module* shuffleModule = Divert::getShuffleModule();
    return getInbound(shuffleModule);
}

void setShuffleOutbound(bool outbound)
{
    Module* shuffleModule = Divert::getShuffleModule();
    return setOutbound(shuffleModule, outbound);
}

bool getShuffleOutbound()
{
    Module* shuffleModule = Divert::getShuffleModule();
    return getOutbound(shuffleModule);
}

void setShuffleChance(float chance)
{
    Module* shuffleModule = Divert::getShuffleModule();
    setChance(shuffleModule, chance);
}

bool getShuffleChance()
{
    Module* shuffleModule = Divert::getShuffleModule();
    return getChance(shuffleModule);
}

void setShuffleMinPackets(unsigned int minPackets)
{
    Module* shuffleModule = Divert::getShuffleModule();
    if (shuffleModule != nullptr)
    {
        showError();
        return;
    }
    shuffleModule->setNum(minPackets);
}

unsigned int getShuffleMinPackets()
{
    Module* shuffleModule = Divert::getShuffleModule();
    
    if (shuffleModule == nullptr)
    {
        showError();
        return 0;
    }

    return (unsigned int)shuffleModule->getNum();
}