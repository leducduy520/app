#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

#include <iostream>
#include <memory>
#include "module_manager.h"
#include "magic_enum.hpp"

using namespace std;

enum class ModuleName : int
{
    GAMES = 0,
    CALCULATOR,
    NOTE,
    ENGLISH
};

void init()
{ 
	REGISTER_MODULE_LOCATION(Games, games);
}

int main()
{
    init();
    unique_ptr<ModuleInterface> mdinterface;
    string task;
    std::cout << "Please enter the task you want to run: ";
    std::cin >> task;
    for (auto& cha : task)
    {
        cha = toupper(cha); // Convert to lowercase for case-insensitive comparison.
    }
    auto etask = magic_enum::enum_cast<ModuleName>(task);
    if (etask.has_value())
    {
        switch (etask.value())
        {
        case ModuleName::GAMES:
        {
            try
            {
                mdinterface = ModuleFactory::Instance()->CreateModule("Games");
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            catch (...)
            {
                std::cerr << "Unknown exception occurred\n";
            }
        }
        break;
        default:
            break;
        }
    }
    else
    {
        std::cerr << "Invalid task entered\n";
    }
    if (mdinterface)
    {
        mdinterface->execute();
    }
    std::cout << "Program finished\n";
    return 0;
}
