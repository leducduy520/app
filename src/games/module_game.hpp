#pragma once
#include "game_interface.h"
#include "module_manager.h"
#include <string>
#include <queue>

class ModuleGame : public ModuleInterface
{
public:
private:
    std::queue<std::future<void>> m_results;

public:
    ModuleGame();
    void execute() override;
    void shutdown() override;
};
