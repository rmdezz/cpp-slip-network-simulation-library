#include "DuplicateHelper.h"
#include "NetSimulatorAPI.h"
#include "ModuleHelper.h"

void SetDuplicate(bool enabled, bool inbound, bool outbound, float chance, unsigned int numCopies)
{
    Module* module = Divert::getDuplicateModule();
    if (!isModuleValid(module)) return;
    if (!isConfigValid(chance, SimulationType::Duplicate)) return;
    Config config = getConfig(enabled, inbound, outbound, chance, 0, numCopies, false);
    configureModule(module, config);
}

void enableDuplicate()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    enableModule(duplicateModule);
}

void disableDuplicate()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    disableModule(duplicateModule);
}

bool isDuplicateEnabled()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    return isModuleEnabled(duplicateModule);
}

void setDuplicateInbound(bool inbound)
{
    Module* duplicateModule = Divert::getDuplicateModule();
    return setInbound(duplicateModule, inbound);
}

bool getDuplicateInbound()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    return getInbound(duplicateModule);
}

void setDuplicateOutbound(bool outbound)
{
    Module* duplicateModule = Divert::getDuplicateModule();
    return setOutbound(duplicateModule, outbound);
}

bool getDuplicateOutbound()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    return getOutbound(duplicateModule);
}

void setDuplicateChance(float chance)
{
    Module* duplicateModule = Divert::getDuplicateModule();
    setChance(duplicateModule, chance);
}

bool getDuplicateChance()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    return getChance(duplicateModule);
}

void setDuplicateCopies(unsigned int copies)
{
    Module* duplicateModule = Divert::getDuplicateModule();
    if (duplicateModule != nullptr)
    {
        showError();
        return;
    }

    duplicateModule->setNum(copies);
}

unsigned int getDuplicateCopies()
{
    Module* duplicateModule = Divert::getDuplicateModule();
    if (duplicateModule != nullptr)
    {
        showError();
        return 0;
    }
    return (unsigned int)duplicateModule->getNum();
}