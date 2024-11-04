#include "pre-definition.hpp"

int main()
{
    init();
    std::unique_ptr<ModuleInterface> mdinterface;
    std::string task;
    while(true)
    {
        std::cout << "Please enter the task you want to run: ";
        std::cin >> task;
        for (auto& cha : task)
        {
            cha = static_cast<char>(toupper(cha));
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
        if (mdinterface)
        {
            mdinterface->execute();
        }
    }
    std::cout << "Program finished\n";
    return 0;
}
