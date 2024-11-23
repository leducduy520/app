#include "mongo_db_client.hpp"
#include "pre-definition.hpp"
#include "utilities.hpp"
#include <boost/predef.h>    // For platform and architecture detection
#include <boost/version.hpp> // For Boost version
#include <thread>            // For hardware concurrency

int main()
{
    init();
    INDEBUG(printSystemInfo())
    ModuleManager::getInstance()->genNewSession();
    std::string task;
    while (true)
    {
        ask_for_task(task);
        auto etask = magic_enum::enum_cast<ModuleName>(task);
        if (etask.has_value())
        {
            std::shared_ptr<ModuleInterface> mdinterface = nullptr;
            std::weak_ptr<ModuleInterface> winterface;
            auto moduleid = toModuleId(etask.value());
            switch (etask.value())
            {
            case ModuleName::GAMES:
            case ModuleName::CALCULATOR:
            case ModuleName::TRANSLATE:
            {
                if (ModuleManager::getInstance()->loadModule(moduleid))
                {
                    mdinterface = ModuleManager::getInstance()->getModuleClass(moduleid);
                    winterface = mdinterface;
                }
            }
            break;
            case ModuleName::REMOVE:
            {
                std::cout << "Type the name of the module you want to remove: ";
                std::cin >> moduleid;
                ModuleManager::getInstance()->releaseModuleInstance(moduleid);
            }
            break;
            default:
                break;
            }
            if (auto minterface = winterface.lock())
            {
                minterface->execute();
            }
        }
        else if (task == "QUIT")
        {
            break;
        }
        else
        {
            std::cerr << "Invalid task entered\n";
        }
    }
    ModuleManager::getInstance()->endSession();
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

void printSystemInfo()
{

    std::cout << "===== System Information =====" << std::endl;

    // Operating system
    std::cout << "Operating System: ";
#if BOOST_OS_WINDOWS
    std::cout << "Windows";
#elif BOOST_OS_LINUX
    std::cout << "Linux";
#elif BOOST_OS_MACOS
    std::cout << "macOS";
#elif BOOST_OS_UNIX
    std::cout << "Unix";
#else
    std::cout << "Unknown";
#endif
    std::cout << std::endl;

    // Architecture
    std::cout << "Processor Architecture: ";
#if BOOST_ARCH_X86_64
    std::cout << "x86_64";
#elif BOOST_ARCH_ARM
    std::cout << "ARM";
#elif BOOST_ARCH_PPC
    std::cout << "PowerPC";
#elif BOOST_ARCH_MIPS
    std::cout << "MIPS";
#else
    std::cout << "Unknown";
#endif
    std::cout << std::endl;

    // Compiler
    std::cout << "Compiler: ";
#if BOOST_COMP_MSVC
    std::cout << "MSVC";
#elif BOOST_COMP_GNUC
    std::cout << "GCC";
#elif BOOST_COMP_CLANG
    std::cout << "Clang";
#else
    std::cout << "Unknown";
#endif
    std::cout << std::endl;

    // Number of CPU cores
    std::cout << "CPU Cores: " << std::thread::hardware_concurrency() << std::endl;

    // Boost library version
    std::cout << "Boost Library Version: " << BOOST_LIB_VERSION << std::endl;

    std::cout << "==============================" << std::endl;
}
