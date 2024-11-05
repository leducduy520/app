#include "module_game.hpp"
#include "game_factory.h"
#include "thread_pool.hpp"
#include "gameA.hpp"
#include "gameB.hpp"
#include <exception>

REGISTER_MODULE_CLASS(ModuleGame, NAME)

ModuleGame::ModuleGame() : ModuleInterface(NAME)
{
    REGISTAR_GAME(gameA, A);
    REGISTAR_GAME(gameB, B);
}

void ModuleGame::execute()
{
    std::string gameId;
    std::cout << "A: game A\nB: game B\n";
    std::cout << "Please input a game ID: ";
    std::cin >> gameId;
    auto* game = GameFactory::Instance()->createGame(gameId);
    if (game != nullptr)
    {
        std::cout << "Game created: " << gameId << '\n';
        m_result = ThreadPool::getInstance()->submit(100, [game]() {
            game->run();
            delete game;
        });
        std::cout << "Game run: " << gameId << '\n';
    }
    else
    {
        std::cerr << "Failed to create game" << '\n';
    }
}

void ModuleGame::shutdown()
{
    if(m_result.valid())
    {
        m_result.get();
    }
    m_finished = true;
}
