#include "module_game.hpp"
#include "game_factory.h"
#include "gameA.hpp"
#include "gameB.hpp"
#include <exception>

REGISTER_MODULE_CLASS(ModuleGame, NAME)

static unsigned int count_ref = 0;

ModuleGame::ModuleGame() : ModuleInterface(NAME)
{
    ++count_ref;
    REGISTAR_GAME(gameA, A);
    REGISTAR_GAME(gameB, B);
}

void ModuleGame::execute()
{
    printf("count ref: %d\n", count_ref);
    std::string gameId;
    std::cout << "A: game A\nB: game B\n";
    std::cout << "Please input a game ID: ";
    std::cin >> gameId;
    auto* game = GameFactory::Instance()->createGame(gameId);
    if (game == nullptr)
    {
        std::cout << "Game not found!\n";
        return;
    }
    game->run();
    delete game;
}

void ModuleGame::shutdown()
{
    std::cout << "Game shutdown\n";
}
