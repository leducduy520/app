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
        ask_for_task(task);
        auto etask = magic_enum::enum_cast<ModuleName>(task);
        if (etask.has_value())
        {
            ModuleInterface* mdinterface = nullptr;
            auto moduleid = toModuleId(etask.value());
            switch (etask.value())
            {
            case ModuleName::GAMES:
            case ModuleName::CALCULATOR:
            case ModuleName::TRANSLATE:
            {
                if (ModuleManager::getInstance()->loadModule(moduleid))
                {
                    mdinterface = ModuleManager::getInstance()->getInterface(moduleid);
                }
            }
            break;
            case ModuleName::REMOVE:
            {
                std::cout << "Type the name of the module you want to remove: ";
                std::cin >> moduleid;
                ModuleManager::getInstance()->releaseModule(moduleid);
            }
            break;
            default:
                break;
            }
            if (mdinterface != nullptr)
            {
                mdinterface->execute();
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
        }
    }
    std::cout << "Program finished\n";
    return 0;
}

void ask_for_task(std::string& task)
{
    std::cout << "Please enter the task you want to run:\n";
    std::cout << "Available tasks:\n";
    for (const auto& value : magic_enum::enum_values<ModuleName>())
    {
        std::string mid{magic_enum::enum_name(value)};
        tolower_str(mid);
        std::cout << "  -  " << mid << '\n';
    }
    std::cin >> task;
    toupper_str(task);
}
