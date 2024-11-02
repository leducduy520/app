#include "game_interface.h"
#include "game_factory.h"
#include "gameA.hpp"
#include "gameB.hpp"

extern "C"
{
    IGame* createGame(const char* name)
    {
        return GameFactory::Instance()->createGame(name);
    }
}
