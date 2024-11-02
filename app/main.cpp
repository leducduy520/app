#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

#include <iostream>
#include <memory>
#include "game_interface.h"
#include "module_game.hpp"

using namespace std;

static std::filesystem::path getExecutablePath()
{
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
#endif
}

#define EXECUTABLE_PATH getExecutablePath()

int main()
{
    unique_ptr<ModuleInterface> mdinterface;
    string task;
    std::cout << "Please enter the task you want to run: ";
    std::cin >> task;
    
    if (task == "game")
    {
        try
        {
            mdinterface.reset(new ModuleGame());
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        catch(...)
        {
            std::cerr << "Unknown exception occurred\n";
        }
    }
    if(mdinterface)
    {
        mdinterface->execute();
    }
    std::cout << "Program finished\n";
    return 0;
}
