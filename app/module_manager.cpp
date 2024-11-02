#include "module_manager.h"

ModuleManager* ModuleManager::m_instance = nullptr;
// Register a module with its expected path
void ModuleManager::registerModule(const std::string& moduleName, const std::string& modulePath)
{
    modulePaths[moduleName] = modulePath;
}

// Load a registered module
bool ModuleManager::loadModule(const std::string& moduleName)
{
    auto it = modulePaths.find(moduleName);
    if (it == modulePaths.end())
    {
        std::cerr << "Module not registered: " << moduleName << std::endl;
        return false;
    }

    LibraryHandle hModule = LoadLibraryFunction(it->second.c_str());
    if (hModule == nullptr)
    {
#ifdef _WIN32
        std::cerr << "Failed to load module: " << moduleName << " Error: " << GetLastError() << std::endl;
#else
        std::cerr << "Failed to load module: " << moduleName << " Error: " << dlerror() << std::endl;
#endif
        return false;
    }

    loadedModules[moduleName] = hModule;
    std::cout << "Loaded module: " << it->second << std::endl;
    return true;
}

// Get a function pointer from a loaded module
FunctionAddress ModuleManager::getModuleMethod(const std::string& moduleName, const std::string& functionName)
{
    auto it = loadedModules.find(moduleName);
    if (it == loadedModules.end())
    {
        std::cerr << "Module not loaded: " << moduleName << std::endl;
        return nullptr;
    }

    FunctionAddress func = GetFunctionAddress(it->second, functionName.c_str());
    if (func == nullptr)
    {
        std::cerr << "Failed to get function: " << functionName << " from module: " << moduleName << std::endl;
    }

    return func;
}

// Release a loaded module
void ModuleManager::releaseModule(const std::string& moduleName)
{
    auto it = loadedModules.find(moduleName);
    if (it != loadedModules.end())
    {
        UnloadLibrary(it->second);
        std::cout << "Released module: " << modulePaths[moduleName] << std::endl;
        loadedModules.erase(it);
    }
    else
    {
        std::cerr << "Module not loaded: " << moduleName << std::endl;
    }
}

// Destructor to ensure all modules are released
ModuleManager::~ModuleManager()
{
    for (const auto& pair : loadedModules)
    {
        UnloadLibrary(pair.second);
        std::cout << "Released module in destructor: " << modulePaths[pair.first] << std::endl;
    }
    loadedModules.clear();
}

// Helper function to get the actual path of a loaded module (DLL)
std::string ModuleManager::getModulePath(const std::string& moduleName)
{
    auto it = loadedModules.find(moduleName);
    if (it == loadedModules.end())
    {
        std::cerr << "Module not loaded: " << moduleName << std::endl;
        return "";
    }

    LibraryHandle hModule = it->second;

#ifdef _WIN32
    char path[MAX_PATH];
    if (GetModuleFileName(hModule, path, MAX_PATH) == 0)
    {
        std::cerr << "Failed to get module path. Error: " << GetLastError() << std::endl;
        return "";
    }
    return std::string(path);
#else
    Dl_info info;
    if (dladdr(hModule, &info) == 0)
    {
        std::cerr << "Failed to get module path. Error: " << dlerror() << std::endl;
        return "";
    }
    return std::string(info.dli_fname);
#endif
}

ModuleManager* ModuleManager::getInstance()
{
    static bool construct = false;
    if(!construct)
    {
        m_instance = new ModuleManager();
        construct = true;
    }
    return m_instance;
}
