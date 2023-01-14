#include "LagHelper.h"
#include "NetSimulatorAPI.h"
#include "Divert.h"
#include "Module.h"
#include "ModuleHelper.h"

    void SetLag(bool enabled, bool inbound, bool outbound, float delay)
    {
        Module* lagModule = Divert::getLagModule();

        if (!isModuleValid(lagModule)) return;
        if (!isConfigValid(delay, SimulationType::Lag)) return;

        Config lagConfig = getConfig(enabled, inbound, outbound, 0, delay, 0, false);
        configureModule(lagModule, lagConfig);
    }

    void enableLag()
    {
        Module* lagModule = Divert::getLagModule();
        enableModule(lagModule);
    }

    void disableLag()
    {
        Module* lagModule = Divert::getLagModule();
        disableModule(lagModule);
    }

    bool isLagEnabled()
    {
        Module* lagModule = Divert::getLagModule();
        return isModuleEnabled(lagModule);
    }

    void setLagDelay(float delay)
    {
        Module* lagModule = Divert::getLagModule();
        setDelay(lagModule, delay);
        //cout << "lag delay: " << lagModule->getTime() << endl;
    }

    float getLagDelay()
    {
        Module* lagModule = Divert::getLagModule();
        return getDelay(lagModule);
    }

    void setLagInbound(bool inbound)
    {
        Module* lagModule = Divert::getLagModule();
        return setInbound(lagModule, inbound);
    }

    bool getLagInbound()
    {
        Module* lagModule = Divert::getLagModule();
        return getInbound(lagModule);
    }

    void setLagOutbound(bool outbound)
    {
        Module* lagModule = Divert::getLagModule();
        return setOutbound(lagModule, outbound);
    }

    bool getLagOutbound()
    {
        Module* lagModule = Divert::getLagModule();
        return getOutbound(lagModule);
    }