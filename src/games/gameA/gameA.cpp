#include "gameA.hpp"
#include "thread_pool.hpp"

gameA::gameA()
{}

void gameA::run()
{
    cout << "\ngameA is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    cout << "gameA has finished running\n";
}
