#include "module_game.hpp"
#include <exception>

ModuleGame::ModuleGame()
    :ModuleInterface("Games", "games")
{
    m_createGameFunc = (CreateGameFunc)ModuleManager::getInstance()->getModuleMethod(m_moduleName, "createGame");
}

ModuleGame::~ModuleGame()
{
    
}

void ModuleGame::execute()
{
    std::string gameId;
    std::cout << "Please input a game ID: ";
    std::cin >> gameId;
    auto* game = m_createGameFunc(gameId.c_str());
    if (game)
    {
        // Implement game logic here
        // Example: game->run();
        game->run();
        delete game;
    }
    else
    {
        std::cerr << "Failed to create game" << std::endl;
    }
}
