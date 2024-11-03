#pragma once
#include "game_factory.h"
using namespace std;
class gameB : public IGame
{
private:
    REGISTAR_GAME_DECLARE(gameB);
public:
    gameB();
    void run() override;
};
