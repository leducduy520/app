#include "mongo_db_client.hpp"
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
        m_results.push(ThreadPool::getInstance()->submit(100, [game]() {
            game->run();
            delete game;
        }));
        std::cout << "Game run: " << gameId << '\n';
    }
    else
    {
        std::cerr << "Failed to create game" << '\n';
    }
}

void ModuleGame::shutdown()
{
    std::cout << "Game shutdown\n";
    while (!m_results.empty())
    {
        if (m_results.front().valid())
        {
            m_results.front().get();
        }
        m_results.pop();
    }
}
