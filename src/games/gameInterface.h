#pragma once

class IGame
{
public:
    virtual void run() = 0;
    virtual ~IGame() = default;
};

#if defined _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

extern "C"
{
    EXPORT_API IGame* createGameA();
    EXPORT_API IGame* createGameB();
    EXPORT_API void destroyGame(IGame* game);
}
