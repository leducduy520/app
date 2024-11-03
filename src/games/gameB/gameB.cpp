#include "gameB.hpp"
#include "thread_pool.hpp"
gameB::gameB()
{
    cout << "gameB default constructor\n";
}

void gameB::run()
{
    cout << "gameB is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    cout << "gameB has finished running\n";
}
