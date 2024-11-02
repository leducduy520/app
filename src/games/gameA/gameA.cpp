#include "gameA.hpp"
#include "game_factory.h"
#include <iostream>

REGISTER_GAME(gameA, "A");

gameA::gameA()
{
    cout << "gameA default constructor\n";
}

void gameA::run()
{
    cout << "gameA is running\n";
}
