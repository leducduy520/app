#include "module_manager.h"
#include "utilities.hpp"

using namespace std::chrono;

namespace dld
{
    // Register a module with its expected path
    void ModuleManager::registerModule(const std::string& moduleName, const std::string& modulePath)
    {
        std::cout << "Registering module " << moduleName << " with " << modulePath << '\n';
        m_module_paths[moduleName] = modulePath;
    }

    // Load a registered module
    bool ModuleManager::loadModule(const std::string& moduleName)
    {
        auto pit = m_module_paths.find(moduleName);
        if (pit == m_module_paths.end())
        {
            std::cerr << "Module not registered: " << moduleName << '\n';
            return false;
        }

        auto hModuleit = m_module_handle.find(moduleName);
        if (hModuleit == m_module_handle.end())
        {
            LibraryHandle hModule = LoadLibraryFunction(pit->second.c_str());
            if (hModule == nullptr)
            {
#ifdef _WIN32
                std::cerr << "Failed to load module lib: " << moduleName << " Error: " << GetLastError() << '\n';
#else
                std::cerr << "Failed to load module lib: " << moduleName << " Error: " << dlerror() << '\n';
#endif
                return false;
            }
            m_module_handle[moduleName] = hModule;
        }
        std::cout << "Loaded module: " << pit->second << '\n';
        return true;
    }

    // Get a function pointer from a loaded module
    FunctionAddress ModuleManager::getModuleMethod(const std::string& moduleName, const std::string& functionName)
    {
        auto pit = m_module_handle.find(moduleName);
        if (pit == m_module_handle.end())
        {
            std::cerr << "Module not loaded: " << moduleName << '\n';
            return nullptr;
        }

        FunctionAddress func = GetFunctionAddress(pit->second, functionName.c_str());
        if (func == nullptr)
        {
            std::cerr << "Failed to get function: " << functionName << " from module: " << moduleName << '\n';
        }

        return func;
    }

    // Release a loaded module
    void ModuleManager::releaseModuleLib(const std::string& moduleName)
    {
        if (m_module_handle.empty())
        {
            std::cerr << "No loaded modules\n";
            return;
        }
        auto pit = m_module_handle.find(moduleName);
        if (pit != m_module_handle.end())
        {
            m_factory.releaseModule(moduleName);
            UnloadLibrary(pit->second);
            m_module_handle.erase(pit);
            std::cout << "Released module lib: " << m_module_paths[moduleName] << '\n';
        }
        else
        {
            std::cerr << "Module lib not loaded: " << moduleName << '\n';
        }
    }

    void ModuleManager::releaseModuleInstance(const std::string& moduleName)
    {
        this->m_factory.releaseModule(moduleName);
    }

    // Destructor to ensure all modules are released
    ModuleManager::~ModuleManager()
    {
        m_factory.release();
        for (const auto& pair : m_module_handle)
        {
            UnloadLibrary(pair.second);
        }
        m_module_handle.clear();
    }

    // Helper function to get the actual path of a loaded module (DLL)
    std::string ModuleManager::getModulePath(const std::string& moduleName)
    {
        auto pit = m_module_handle.find(moduleName);
        if (pit == m_module_handle.end())
        {
            std::cerr << "Module lib not loaded: " << moduleName << '\n';
            return "";
        }

        LibraryHandle hModule = pit->second;

#ifdef _WIN32
        char path[MAX_PATH];
        if (GetModuleFileName(hModule, path, MAX_PATH) == 0)
        {
            // std::cerr << "Failed to get module path. Error: " << GetLastError() << '\n';
            return m_module_paths[moduleName];
        }
        return {path};
#else
        Dl_info info;
        if (dladdr(hModule, &info) == 0)
        {
            // std::cerr << "Failed to get module path. Error: " << dlerror() << '\n';
            return m_module_paths[moduleName];
        }
        return {info.dli_fname};
#endif
    }

    std::shared_ptr<ModuleInterface> ModuleManager::getModuleInstance(const std::string& moduleName)
    {
        auto pit = m_module_handle.find(moduleName);
        if (pit == m_module_handle.end() && !this->loadModule(moduleName))
        {
            std::cerr << "Module not loaded: " << moduleName << '\n';
            return nullptr;
        }
        return m_factory.createModule(moduleName);
    }

    ModuleManager* ModuleManager::getInstance()
    {
        static ModuleManager instance;
        return &instance;
    }

    void ModuleManager::endSession()
    {
        m_factory.release();
    }

    ModuleInterface::ModuleInterface(std::string modulename)
        : m_moduleName(std::move(modulename)){INDEBUG(std::cout << "Module " << m_moduleName << '\n')}

          ModuleInterface::~ModuleInterface()
    {
        INDEBUG(std::cout << "~Module " << m_moduleName << '\n')
    }
} // namespace dld
