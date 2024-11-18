#include "gameB.hpp"
#include "thread_pool.hpp"
#include <iostream>

gameB::gameB()
{}

void gameB::run()
{
    // std:cout << "\ngameB is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    // std:cout << "gameB has finished running\n";
}
