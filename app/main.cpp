#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif


#include "module_manager.h"

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
    ModuleManager moduleManager;
    std::cout << EXECUTABLE_PATH << std::endl;
#if defined(_WIN32)
    moduleManager.registerModule("PingPongGame", "pingpong_game.dll");
#else
    moduleManager.registerModule("PingPongGame", "pingpong_game.so");
#endif
    auto execPath = EXECUTABLE_PATH;
    if (moduleManager.loadModule("PingPongGame"))
    {
        // Retrieve function pointers for creating and destroying the Game object
        // using CreateGameFunc = IGame* (*)();
        // using DestroyGameFunc = void (*)(IGame*);

        // CreateGameFunc createGame =
        //     reinterpret_cast<CreateGameFunc>(moduleManager.getFunction("PingPongGame", "createPingPongGame"));
        // DestroyGameFunc destroyGame =
        //     reinterpret_cast<DestroyGameFunc>(moduleManager.getFunction("PingPongGame", "destroyGame"));

        // if (createGame && destroyGame)
        // {
        //     // Create the Game object
        //     IGame* game = createGame();
        //     if (game)
        //     {
        //         std::string path = (getExecutablePath() / ".." / "resources" / "").string();
        //         game->init(path);
        //         game->run();
        //         // Destroy the Game object
        //         destroyGame(game);
        //     }
        // }

        // // Release module
        // moduleManager.releaseModule("PingPongGame");
    }
    return 0;
}
