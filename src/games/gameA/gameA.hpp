#pragma once
#include "game_factory.h"
using namespace std;

class gameA : public IGame
{
private:
public:
    gameA();
    void run() override;
};
