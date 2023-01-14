#include "EncryptHelper.h"
#include "NetSimulatorAPI.h"
#include "ModuleHelper.h"

void SetEncrypt(bool enabled, bool inbound, bool outbound, float chance)
{
    Module* encryptModule = Divert::getEncryptModule();
    if (!isModuleValid(encryptModule)) return;
    if (!isConfigValid(chance, SimulationType::Encrypt)) return;
    Config encryptConfig = getConfig(enabled, inbound, outbound, chance, 0, 0, false);
    configureModule(encryptModule, encryptConfig);
}

void enableEncrypt()
{
    Module* encryptModule = Divert::getEncryptModule();
    enableModule(encryptModule);
}

void disableEncrypt()
{
    Module* encryptModule = Divert::getEncryptModule();
    disableModule(encryptModule);
}

bool isEncryptEnabled()
{
    Module* encryptModule = Divert::getEncryptModule();
    return isModuleEnabled(encryptModule);
}

void setEncryptInbound(bool inbound)
{
    Module* encryptModule = Divert::getEncryptModule();
    return setInbound(encryptModule, inbound);
}

bool getEncryptInbound()
{
    Module* encryptModule = Divert::getEncryptModule();
    return getInbound(encryptModule);
}

void setEncryptOutbound(bool outbound)
{
    Module* encryptModule = Divert::getEncryptModule();
    return setOutbound(encryptModule, outbound);
}

bool getEncryptOutbound()
{
    Module* encryptModule = Divert::getEncryptModule();
    return getOutbound(encryptModule);
}

void setEncryptChance(float chance)
{
    Module* encryptModule = Divert::getEncryptModule();
    setChance(encryptModule, chance);
}

bool getEncryptChance()
{
    Module* encryptModule = Divert::getEncryptModule();
    return getChance(encryptModule);
}