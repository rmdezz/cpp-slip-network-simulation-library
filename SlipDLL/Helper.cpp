#include "Helper.h"

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

        // Toggle module
        modules[(int)config.type]->setEnabledFlag(config.enabled);

        // Set configs
        setModuleConfigs(modules[(int)config.type], config);
    }
}

void setModuleConfigs(Module* module, Config config)
{
    module->setInbound(config.inbound);
    module->setOutbound(config.outbound);
    module->setChance(config.chance);
    module->setTime(config.delay);
    module->setNum(config.num);
}