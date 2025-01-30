#pragma once

#include "game_interface.h"
#include <string>
#include <functional>
#include <iostream>

class GameFactory
{

public:
    using CreateGameFunc = std::function<IGame*(void)>;
    IGame* createGame(const std::string& gameId);
    void registerGame(const std::string&, CreateGameFunc createfunc);
    static GameFactory* Instance();

private:
    std::unordered_map<std::string, CreateGameFunc> m_games;
};

template <class T>
class GameRegistrar
{
public:
    GameRegistrar(const std::string& gameId)
    {
        GameFactory::Instance()->registerGame(gameId, []() { return new T(); });
    }
};

#define REGISTAR_GAME(gameclass, gameId, ...)                                                                          \
    GameFactory::Instance()->registerGame(#gameId, []() { return new gameclass(__VA_ARGS__); });
