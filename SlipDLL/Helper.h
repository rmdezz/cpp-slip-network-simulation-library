#pragma once
#include "Divert.h"
#include "slipDLL.h"
#include <iostream>
#include <vector>

void disableStandardStreamSync();
void printConfig(Config config);
void configureModules(Divert& divert, Config* configs, int length);
void setModuleConfigs(Module* module, Config config);
