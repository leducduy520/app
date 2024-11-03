#include "gameB.hpp"
#include "mongo_db_client.hpp"

REGISTAR_GAME_DEFINE(gameB, B);

gameB::gameB()
{
    cout << "gameB default constructor\n";
    DBClient::GetInstance();
}

void gameB::run()
{
    cout << "gameB is running...\n";
}
