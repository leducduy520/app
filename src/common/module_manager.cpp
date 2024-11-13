#include "module_manager.h"

std::unique_ptr<ModuleManager> ModuleManager::m_instance = nullptr;
std::once_flag ModuleManager::m_flag;

// Register a module with its expected path
void ModuleManager::registerModule(const std::string& moduleName, const std::string& modulePath)
{
    std::cout << "Registering module " << moduleName << " with " << modulePath << '\n';
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

    auto hModuleit = loadedModuleLibs.find(moduleName);
    if (hModuleit == loadedModuleLibs.end())
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
        loadedModuleLibs[moduleName] = hModule;
    }
    std::cout << "Loaded module: " << pit->second << '\n';
    return true;
}

// Get a function pointer from a loaded module
FunctionAddress ModuleManager::getModuleMethod(const std::string& moduleName, const std::string& functionName)
{
    auto pit = loadedModuleLibs.find(moduleName);
    if (pit == loadedModuleLibs.end())
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
    if (loadedModuleLibs.empty())
    {
        std::cerr << "No loaded modules\n";
        return;
    }
    auto pit = loadedModuleLibs.find(moduleName);
    if (pit != loadedModuleLibs.end())
    {
        Factory.releaseModule(moduleName);
        UnloadLibrary(pit->second);
        loadedModuleLibs.erase(pit);
        std::cout << "Released module lib: " << modulePaths[moduleName] << '\n';
    }
    else
    {
        std::cerr << "Module lib not loaded: " << moduleName << '\n';
    }
}

void ModuleManager::releaseModuleClass(const std::string& moduleName)
{
    this->Factory.releaseModule(moduleName);
}

// Destructor to ensure all modules are released
ModuleManager::~ModuleManager()
{
    Factory.release();
    for (const auto& pair : loadedModuleLibs)
    {
        UnloadLibrary(pair.second);
    }
    loadedModuleLibs.clear();
}

// Helper function to get the actual path of a loaded module (DLL)
std::string ModuleManager::getModulePath(const std::string& moduleName)
{
    auto pit = loadedModuleLibs.find(moduleName);
    if (pit == loadedModuleLibs.end())
    {
        std::cerr << "Module lib not loaded: " << moduleName << '\n';
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

std::shared_ptr<ModuleInterface> ModuleManager::getModuleClass(const std::string& moduleName)
{
    auto pit = loadedModuleLibs.find(moduleName);
    if(pit != loadedModuleLibs.end())
    {
        return Factory.createModule(moduleName);
    }
    std::cerr << "Module not loaded: " << moduleName << '\n';
    return nullptr;
}

ModuleManager* ModuleManager::getInstance()
{
    std::call_once(m_flag, []() { m_instance = std::make_unique<ModuleManager>(); });
    return m_instance.get();
}

ModuleInterface::ModuleInterface(std::string modulename) : m_moduleName(std::move(modulename))
{}

std::shared_ptr<ModuleInterface> ModuleFactory::createModule(const std::string& moduleName)
{
    auto fit = m_modules.find(moduleName);
    if (fit != m_modules.end())
    {
        if(auto search = loadedModuleClasses.find(moduleName); search != loadedModuleClasses.end())
        {
            return loadedModuleClasses[moduleName];
        }
        return fit->second();
    }
    std::cerr << "Module not registered: " << moduleName << '\n';
    return nullptr;
}

void ModuleFactory::registerModule(const std::string& moduleName, std::function<std::shared_ptr<ModuleInterface>(void)> createFunc)
{
    m_modules[moduleName] = std::move(createFunc);
}

void ModuleFactory::releaseModule(const std::string& moduleName)
{
    auto fit = loadedModuleClasses.find(moduleName);
    if (fit!= loadedModuleClasses.end())
    {
        fit->second->shutdown();
        loadedModuleClasses.erase(fit);
    }
}

void ModuleFactory::release()
{
    for (const auto& pair : loadedModuleClasses)
    {
        pair.second->shutdown();
    }
    loadedModuleClasses.clear();
    m_modules.clear();
}
