#pragma once

#include "game_interface.h"
#include <string>
#include <functional>
#include <iostream>

class GameFactory;

using namespace std;

using CreateGameFunc = function<IGame*(void)>;

class GameFactory{
    unordered_map<string, CreateGameFunc> m_games;

public:
    IGame* createGame(const std::string &gameId);
    void registerGame(const std::string &, CreateGameFunc createfunc);
    static GameFactory* Instance();
};

template<class T>
class RegisterGame{
public:
    RegisterGame(const std::string &gameId)
    {
        GameFactory::Instance()->registerGame(gameId, [](){ return new T();});
    }
};


#define REGISTER_GAME(gameclass, gameId) \
    static RegisterGame<gameclass> register##gameclass(gameId);
