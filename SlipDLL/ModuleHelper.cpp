#include "ModuleHelper.h"
#include "NetSimulatorAPI.h"

void disableStandardStreamSync()
{
    // Disable synchronization between the standard
    // C++ streams and the standard C streams
    // to improve performance
    std::ios_base::sync_with_stdio(false);
}

void printConfig(Config config)
{
    std::cout << "-----------------------------" << std::endl;
    std::cout << "enabled: " << config.enabled << std::endl;
    std::cout << "inbound: " << config.inbound << std::endl;
    std::cout << "outbound: " << config.outbound << std::endl;
    std::cout << "chance: " << config.chance << std::endl;
    std::cout << "delay: " << config.delay << std::endl;
    std::cout << "num. copies: " << config.num << std::endl;
    std::cout << "drop: " << config.drop << std::endl;
    std::cout << "-----------------------------" << std::endl;
}

/*
*   @brief Configure the simulation modules with the provided settings.
*   @param divert A reference to the Divert object used to manage the simulation.
*   @param configs A list of Config objects specifying the simulation settings for each feature.
*/
void configureModules(Divert& divert, Config* configs, int length)
{
    // Get the simulation modules
    Module* const* modules = divert.getModules();

    // Iterate over each module and apply the corresponding configuration
    for (int i = 0; i < length; i++)
    {
        Config config = configs[i];
        Module* currentModule = modules[(UINT)config.type];
        configureModule(currentModule, config);
    }
}

void configureModule(Module* module, const Config& config)
{
    // Toggle module
    module->setEnabledFlag(config.enabled);
    // Set configs
    setModuleConfigs(module, config);
}

void setModuleConfigs(Module* module, const Config& config)
{
    module->setInbound(config.inbound);
    module->setOutbound(config.outbound);
    module->setChance(config.chance);
    module->setTime(config.delay);
    module->setNum(config.num);
    module->setDrop(config.drop);
}

void enableModule(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return;
    }

    module->setEnabledFlag(true);
}

void disableModule(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return;
    }
    module->setEnabledFlag(false);
}

bool isModuleEnabled(Module* module)
{
    return module != nullptr && module->getEnabledFlag() == true;
}

void setDelay(Module* module, float delay)
{
    if (module == nullptr)
    {
        showError();
        return;
    }
    else if (delay < 0)
    {
        showNegativeValueError();
        return;
    }
        
    module->setTime(delay);
}

float getDelay(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return 0;
    }

    return module->getTime();
}


void setInbound(Module* module, bool inbound)
{
    if (module == nullptr)
    {
        showError();
        return;
    }
    module->setInbound(inbound);
}

bool getInbound(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return false;
    }
    return module->getInbound();
}

void setOutbound(Module* module, bool outbound)
{
    if (module == nullptr)
    {
        showError();
        return;
    }
    module->setOutbound(outbound);
}

bool getOutbound(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return false;
    }
    return module->getOutbound();
}

void setChance(Module* module, float chance)
{
    if (module == nullptr)
    {
        showError();
        return;
    }
    else if (chance < 0 || chance > 100)
    {
        showOutOfBoundsError();
        return;
    }
    module->setChance(chance);
}

bool getChance(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return 0;
    }

    float chance = module->getChance();

    if (chance < 0 || chance > 100)
        return 0;

    return module->getChance();
}

void showError()
{
    string message = "It appears that the program has not yet been started. Please start the program before attempting to use the modules.";
    MessageBoxA(NULL,
        message.c_str(),
        "Error",
        MB_ICONERROR | MB_OK);
}

void showInvalidParams()
{
    string message = "Invalid input: One or more parameters provided are not valid. Please check your input and try again.";
    MessageBoxA(NULL,
        message.c_str(),
        "Error",
        MB_ICONERROR | MB_OK);
}

void showNegativeValueError()
{
    string message = "Error: Invalid value entered. Please ensure that the value provided is greater than or equal to zero.";
    MessageBoxA(NULL,
        message.c_str(),
        "Error",
        MB_ICONERROR | MB_OK);
}

void showOutOfBoundsError()
{
    string message = "Error: Value is out of range. Please enter a value between 0 and 100.";
    MessageBoxA(NULL,
        message.c_str(),
        "Error",
        MB_ICONERROR | MB_OK);
}

Config getConfig(bool enabled, bool inbound, bool outbound, float chance, float delay, float num, bool drop)
{
    Config config;
    config.enabled = enabled;
    config.inbound = inbound;
    config.outbound = outbound;
    config.chance = chance;
    config.delay = delay;
    config.num = num;
    config.drop = drop;
    return config;
}

bool isModuleValid(Module* module)
{
    if (module == nullptr)
    {
        showError();
        return false;
    }
    return true;
}

bool isConfigValid(float number, SimulationType simulationType)
{
    switch (simulationType)
    {
        case SimulationType::Lag:
        {
            return isDelayValid(number);
        }
        case SimulationType::Drop:
        case SimulationType::Duplicate:
        case SimulationType::Encrypt:
        case SimulationType::Shuffle:
        {
            return isChanceValid(number);
        }
        default:
            return true;
    }
}

bool isConfigValid(float chance, float number)
{
    if (chance < 0 || chance > 100 || number < 0)
    {
        showInvalidParams();
        return false;
    }
    return true;
}

bool isChanceValid(float chance)
{
    if (chance < 0 || chance > 100)
    {
        showInvalidParams();
        return false;
    }
    return true;
}

bool isDelayValid(float delay)
{
    if (delay < 0)
    {
        showInvalidParams();
        return false;
    }
    return true;
}
