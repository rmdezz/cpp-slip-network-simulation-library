#pragma once

#ifndef MODULE_HELPER

#include "Divert.h"
#include "NetSimulatorAPI.h"
#include <iostream>
#include <vector>

#define EXPORT __declspec(dllexport)

void disableStandardStreamSync();
void printConfig(Config config);
void configureModules(Divert& divert, Config* configs, int length);
void setModuleConfigs(Module* module, const Config& config);
void configureModule(Module* module, const Config& config);
Config getConfig(bool enabled, bool inbound, bool outbound, float chance, float delay, float num, bool drop);
void enableModule(Module* module);
void disableModule(Module* module);
bool isModuleEnabled(Module* module);
void setDelay(Module* module, float delay);
float getDelay(Module* module);
void setInbound(Module* module, bool inbound);
bool getInbound(Module* module);
void setOutbound(Module* module, bool outbound);
bool getOutbound(Module* module);
void setChance(Module* module, float chance);
bool getChance(Module* module);
void showError();
void showNegativeValueError();
void showOutOfBoundsError();
bool isModuleValid(Module* module);
void showInvalidParams();
bool isConfigValid(float chance, SimulationType simulationType);
bool isChanceValid(float chance);
bool isConfigValid(float chance, float number);
bool isDelayValid(float delay);

#endif // !MODULE_HELPER



