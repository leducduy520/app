#pragma once

#include "game_interface.h"
#include <string>
#include <functional>
#include <iostream>

class GameFactory;

using namespace std;


class GameFactory{

public:
    using CreateGameFunc = function<IGame*(void)>;
    IGame* createGame(const std::string &gameId);
    void registerGame(const std::string &, CreateGameFunc createfunc);
    static GameFactory* Instance();
private:
    unordered_map<string, CreateGameFunc> m_games;
};

template<class T>
class Registrar {
public:
    Registrar (const std::string &gameId)
    {
        GameFactory::Instance()->registerGame(gameId, [](){ return new T();});
    }
};

#define REGISTAR_GAME_DECLARE(gameclass) static Registrar<gameclass> registrar;
#define REGISTAR_GAME_DEFINE(gameclass, gameId) Registrar<gameclass> gameclass::registrar(#gameId);
