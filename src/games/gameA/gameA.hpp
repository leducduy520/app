#pragma once
#include "game_factory.h"

class gameA : public IGame
{
private:
public:
    gameA();
    void run() override;
};
