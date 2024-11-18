#include "gameA.hpp"
#include "thread_pool.hpp"
#include <iostream>

gameA::gameA()
{}

void gameA::run()
{
    // std:cout << "\ngameA is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    // std:cout << "gameA has finished running\n";
}
