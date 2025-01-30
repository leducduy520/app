#pragma once
#include "game_interface.h"
#include "module_manager.h"
#include <string>
#include <queue>

using namespace dld;

class ModuleGame : public ModuleInterface
{

public:
    ModuleGame();
    void execute() override;
    void shutdown() override;
};
