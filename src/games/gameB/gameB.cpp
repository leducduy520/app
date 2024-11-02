#include "gameB.hpp"
#include "game_factory.h"
#include "mongo_db_client.hpp"
#include <iostream>

REGISTER_GAME(gameB, "B");

gameB::gameB()
{
    cout << "gameB default constructor\n";
    DBClient::GetInstance();
}

void gameB::run()
{
    cout << "gameB is running...\n";
}
