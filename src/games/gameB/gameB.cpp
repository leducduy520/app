#include "gameB.hpp"
#include "game_factory.h"
#include <iostream>

REGISTER_GAME(gameB, "B");

gameB::gameB()
{
    cout << "gameB default constructor\n";
}

void gameB::run()
{
    cout << "gameB is running...\n";
}
