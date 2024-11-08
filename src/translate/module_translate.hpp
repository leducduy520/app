#pragma once

#include "module_manager.h"

class ModuleTranslator : public ModuleInterface
{
public:
    ModuleTranslator();
    void execute() override;
    void shutdown() override;
};
