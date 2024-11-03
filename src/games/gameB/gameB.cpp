#include "gameB.hpp"
#include "mongo_db_client.hpp"

gameB::gameB()
{
    cout << "gameB default constructor\n";
    DBClient::GetInstance();
}

void gameB::run()
{
    cout << "gameB is running...\n";
}
