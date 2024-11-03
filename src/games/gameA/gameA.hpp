#pragma once
#include "game_factory.h"
using namespace std;
class gameA : public IGame
{
private:
    REGISTAR_GAME_DECLARE(gameA);
public:
    gameA();
    void run() override;
};
