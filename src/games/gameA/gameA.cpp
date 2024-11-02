#include "gameA.hpp"
#include "game_factory.h"
#include "mongo_db_client.hpp"
#include <iostream>

REGISTER_GAME(gameA, "A");

gameA::gameA()
{
    cout << "gameA default constructor\n";
    DBClient::GetInstance();
}

void gameA::run()
{
    cout << "gameA is running\n";
}
