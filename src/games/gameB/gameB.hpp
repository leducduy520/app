#pragma once
#include "game_factory.h"
using namespace std;

class gameB : public IGame
{
private:
public:
    gameB();
    void run() override;
};
