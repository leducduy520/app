#include "module_game.hpp"
#include "game_factory.h"
#include "gameA.hpp"
#include "gameB.hpp"
#include <exception>

REGISTER_MODULE_CLASS(ModuleGame, Games)

ModuleGame::ModuleGame()
    :ModuleInterface("Games")
{
    REGISTAR_GAME(gameA, A);
    REGISTAR_GAME(gameB, B);
}

void ModuleGame::execute()
{
    std::string gameId;
    std::cout << "Please input a game ID: ";
    std::cin >> gameId;
    auto* game = GameFactory::Instance()->createGame(gameId);
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
