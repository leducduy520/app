#include "game_factory.h"

IGame* GameFactory::createGame(const std::string& gameId)
{
    auto pit = m_games.find(gameId);
    if (pit != m_games.end())
    {
        return pit->second();
    }
    return nullptr;
}

void GameFactory::registerGame(const std::string& gameId, CreateGameFunc createfunc)
{
    m_games.emplace(gameId, std::move(createfunc));
}

GameFactory* GameFactory::Instance()
{
    static GameFactory instance;
    return &instance;
}
