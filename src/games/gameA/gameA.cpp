#include "gameA.hpp"
#include "mongo_db_client.hpp"

REGISTAR_GAME_DEFINE(gameA, A);

gameA::gameA()
{
    cout << "gameA default constructor\n";
    DBClient::GetInstance();
}

void gameA::run()
{
    cout << "gameA is running\n";
}
