#pragma once
#include "game_interface.h"
#include "module_manager.h"
#include <string>

using CreateGameFunc = IGame* (*)(const char*);

class ModuleGame : public ModuleInterface
{
    CreateGameFunc m_createGameFunc;
public:
    ModuleGame();
    ~ModuleGame();
    void execute() override;
};
