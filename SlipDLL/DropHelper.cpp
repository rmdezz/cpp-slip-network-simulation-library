#include "Module.h"
#include "NetSimulatorAPI.h"
#include "DropHelper.h"
#include "ModuleHelper.h"

void SetDrop(bool enabled, bool inbound, bool outbound, float chance)
{
    Module* dropModule = Divert::getDropModule();

    if (!isModuleValid(dropModule)) return;

    if (!isConfigValid(chance, SimulationType::Drop)) return;

    Config dropConfig = getConfig(enabled, inbound, outbound, chance, 0, 0, false);
    configureModule(dropModule, dropConfig);
}

void enableDrop()
{
    Module* dropModule = Divert::getDropModule();
    enableModule(dropModule);
}

void disableDrop()
{
    Module* dropModule = Divert::getDropModule();
    disableModule(dropModule);
}

bool isDropEnabled()
{
    Module* dropModule = Divert::getDropModule();
    return isModuleEnabled(dropModule);
}

void setDropInbound(bool inbound)
{
    Module* dropModule = Divert::getDropModule();
    return setInbound(dropModule, inbound);
}

bool getDropInbound()
{
    Module* dropModule = Divert::getDropModule();
    return getInbound(dropModule);
}

void setDropOutbound(bool outbound)
{
    Module* dropModule = Divert::getDropModule();
    return setOutbound(dropModule, outbound);
}

bool getDropOutbound()
{
    Module* dropModule = Divert::getDropModule();
    return getOutbound(dropModule);
}

void setDropChance(float chance)
{
    Module* dropModule = Divert::getDropModule();
    setChance(dropModule, chance);
}

bool getDropChance()
{
    Module* dropModule = Divert::getDropModule();
    return getChance(dropModule);
}

