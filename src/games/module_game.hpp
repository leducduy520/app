#pragma once
#include "game_interface.h"
#include "module_manager.h"
#include <string>

class ModuleGame : public ModuleInterface
{
public:
private:
    // CreateGameFunc m_createGameFunc;
public:
    EXPORT_API ModuleGame();
    void execute() override;
};
