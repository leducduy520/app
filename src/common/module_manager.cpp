#include "module_manager.h"

std::unique_ptr<ModuleManager> ModuleManager::m_instance = nullptr;
std::once_flag ModuleManager::m_flag;

// Register a module with its expected path
void ModuleManager::registerModule(const std::string& moduleName, const std::string& modulePath)
{
    modulePaths[moduleName] = modulePath;
}

// Load a registered module
bool ModuleManager::loadModule(const std::string& moduleName)
{
    auto pit = modulePaths.find(moduleName);
    if (pit == modulePaths.end())
    {
        std::cerr << "Module not registered: " << moduleName << '\n';
        return false;
    }

    LibraryHandle hModule{};
    ModuleInterface* hModuleInterface{};
    auto hModuleit = loadedModules.find(moduleName);
    if (hModuleit == loadedModules.end())
    {
        hModule = LoadLibraryFunction(pit->second.c_str());
        hModuleInterface = ModuleFactory::Instance()->CreateModule(moduleName);
        if (hModule == nullptr || hModuleInterface == nullptr)
        {
#ifdef _WIN32
            std::cerr << "Failed to load module: " << moduleName << " Error: " << GetLastError() << '\n';
#else
            std::cerr << "Failed to load module: " << moduleName << " Error: " << dlerror() << '\n';
#endif
            return false;
        }
    }
    else
    {
        std::cout << "Loaded module: " << pit->second << '\n';
        return true;
    }
    loadedModules[moduleName] = hModule;
    loadedIModules[moduleName].reset(hModuleInterface);
    std::cout << "Loaded module: " << pit->second << '\n';
    return true;
}

// Get a function pointer from a loaded module
FunctionAddress ModuleManager::getModuleMethod(const std::string& moduleName, const std::string& functionName)
{
    auto pit = loadedModules.find(moduleName);
    if (pit == loadedModules.end())
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
void ModuleManager::releaseModule(const std::string& moduleName)
{
    auto pit = loadedModules.find(moduleName);
    if (pit != loadedModules.end())
    {
        loadedIModules.erase(moduleName);
        UnloadLibrary(pit->second);
        loadedModules.erase(pit);
        std::cout << "Released module: " << modulePaths[moduleName] << '\n';
    }
    else
    {
        std::cerr << "Module not loaded: " << moduleName << '\n';
    }
}

// Destructor to ensure all modules are released
ModuleManager::~ModuleManager()
{
    loadedIModules.clear();
    for (const auto& pair : loadedModules)
    {
        UnloadLibrary(pair.second);
        std::cout << "Released module in destructor: " << modulePaths[pair.first] << '\n';
    }
    loadedModules.clear();
}

// Helper function to get the actual path of a loaded module (DLL)
std::string ModuleManager::getModulePath(const std::string& moduleName)
{
    auto pit = loadedModules.find(moduleName);
    if (pit == loadedModules.end())
    {
        std::cerr << "Module not loaded: " << moduleName << '\n';
        return "";
    }

    LibraryHandle hModule = pit->second;

#ifdef _WIN32
    char path[MAX_PATH];
    if (GetModuleFileName(hModule, path, MAX_PATH) == 0)
    {
        std::cerr << "Failed to get module path. Error: " << GetLastError() << '\n';
        return "";
    }
    return {path};
#else
    Dl_info info;
    if (dladdr(hModule, &info) == 0)
    {
        std::cerr << "Failed to get module path. Error: " << dlerror() << '\n';
        return "";
    }
    return {info.dli_fname};
#endif
}

ModuleInterface* ModuleManager::getInterface(const std::string& moduleName)
{
    auto mit = loadedIModules.find(moduleName);
    if (mit == loadedIModules.end())
    {
        return nullptr;
    }
    return mit->second.get();
}

ModuleManager* ModuleManager::getInstance()
{
    std::call_once(m_flag, []() { m_instance = std::make_unique<ModuleManager>(); });
    return m_instance.get();
}

ModuleInterface::ModuleInterface(std::string modulename) : m_moduleName(std::move(modulename))
{}

ModuleFactory* ModuleFactory::Instance()
{
    static ModuleFactory instance;
    return &instance;
}

ModuleInterface* ModuleFactory::CreateModule(const std::string& moduleName)
{
    auto fit = m_modules.find(moduleName);
    if (fit != m_modules.end())
    {
        return fit->second();
    }
    std::cerr << "Module not registered: " << moduleName << '\n';
    return nullptr;
}

void ModuleFactory::RegisterModule(const std::string& moduleName, std::function<ModuleInterface*(void)> createFunc)
{
    m_modules[moduleName] = std::move(createFunc);
}
