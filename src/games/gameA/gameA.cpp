#include "gameA.hpp"
#include "mongo_db_client.hpp"

gameA::gameA()
{
    cout << "gameA default constructor\n";
    DBClient::GetInstance();
}

void gameA::run()
{
    cout << "gameA is running\n";
}
