#ifndef __MODULE_MANAGER_H__
#define __MODULE_MANAGER_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#ifdef _WIN32
#include <Windows.h>
typedef HMODULE LibraryHandle;
typedef FARPROC FunctionAddress;
#define LoadLibraryFunction LoadLibrary
#define GetFunctionAddress GetProcAddress
#define UnloadLibrary FreeLibrary
#else
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
typedef void* LibraryHandle;
typedef void* FunctionAddress;
#define LoadLibraryFunction(path) dlopen(path, RTLD_LAZY)
#define GetFunctionAddress dlsym
#define UnloadLibrary dlclose
#endif

class ModuleManager
{
public:
    // Register a module with its expected path

    void registerModule(const std::string& moduleName, const std::string& modulePath);

    // Load a registered module
    bool loadModule(const std::string& moduleName);

    // Get a function pointer from a loaded module
    FunctionAddress getModuleMethod(const std::string& moduleName, const std::string& functionName);

    // Release a loaded module
    void releaseModule(const std::string& moduleName);

    // Get the actual path of a loaded module
    std::string getModulePath(const std::string& moduleName);

    static ModuleManager* getInstance();

    // Destructor to ensure all modules are released
    ~ModuleManager();

private:
    ModuleManager() = default;
    static ModuleManager* m_instance;
    std::unordered_map<std::string, std::string> modulePaths;
    std::unordered_map<std::string, LibraryHandle> loadedModules;
};

class ModuleInterface
{
protected:
    const std::string m_moduleName;

public:
    ModuleInterface(const std::string& moduleName) : m_moduleName(moduleName)
    {}

    virtual ~ModuleInterface()
    {
        ModuleManager::getInstance()->releaseModule(m_moduleName);
    }

    virtual void execute() = 0;
};

class ModuleFactory
{
    std::unordered_map<std::string, std::function<ModuleInterface*(void)>> m_modules;

public:
    static ModuleFactory* Instance();
    std::unique_ptr<ModuleInterface> CreateModule(const std::string& moduleName);
    void RegisterModule(const std::string& moduleName, std::function<ModuleInterface*(void)> createFunc);
};

template <typename T>
class ModuleRegistar
{
public:
    ModuleRegistar(std::string moduleName)
    {
        ModuleFactory::Instance()->RegisterModule(moduleName, []() -> ModuleInterface* { return new T(); });
    }
};

#if defined(_WIN32)
#define REGISTER_MODULE_LOCATION(moduleName, modulePath)                                                               \
    ModuleManager::getInstance()->registerModule(#moduleName, std::string(#modulePath).append(".dll"));
#else
#define REGISTER_MODULE_LOCATION(moduleName, modulePath)                                                               \
    ModuleManager::getInstance()->registerModule(#moduleName, std::string(#modulePath).append(".so"));
#endif

#define REGISTER_MODULE_CLASS(moduleclass, moduleName)                                                                 \
    const ModuleRegistar<moduleclass> register##moduleclass(#moduleName);

#endif // __MODULE_MANAGER_H__
