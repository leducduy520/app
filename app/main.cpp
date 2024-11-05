#include "pre-definition.hpp"

int main()
{
#if defined(__clang__)
    std::cout << "Compiled with Clang\n";
#elif defined(__GNUC__) || defined(__GNUG__)
    std::cout << "Compiled with GCC\n";
#elif defined(_MSC_VER)
    std::cout << "Compiled with MSVC\n";
#elif defined(__INTEL_COMPILER)
    std::cout << "Compiled with Intel Compiler\n";
#else
    std::cout << "Unknown compiler\n";
#endif
    init();
    std::string task;
    while (true)
    {
        ModuleInterface* mdinterface = nullptr;
        std::cout << "Please enter the task you want to run: ";
        std::cin >> task;
        toupper_str(task);
        auto etask = magic_enum::enum_cast<ModuleName>(task);
        if (etask.has_value())
        {
            auto moduleid = toModuleId(etask.value());
            switch (etask.value())
            {
            case ModuleName::GAMES:
            {
                if (ModuleManager::getInstance()->loadModule(moduleid))
                {
                    mdinterface = ModuleManager::getInstance()->getInterface(moduleid);
                }
            }
            break;
            case ModuleName::CALCULATOR:
            {
                if (ModuleManager::getInstance()->loadModule(moduleid))
                {
                    mdinterface = ModuleManager::getInstance()->getInterface(moduleid);
                }
            }
            break;
            default:
                break;
            }
        }
        else if (task == "QUIT")
        {
            std::cout << "Quit\n";
            break;
        }
        else
        {
            std::cerr << "Invalid task entered\n";
            break;
        }
        if (mdinterface != nullptr)
        {
            mdinterface->execute();
        }
    }
    std::cout << "Program finished\n";
    return 0;
}
