#ifndef __MODULE_MANAGER_H__
#define __MODULE_MANAGER_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <future>

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

#if defined(_MSC_VER)
#undef max
#endif

class ModuleFactory;

class ModuleInterface
{
protected:
    const std::string m_moduleName;

public:
    ModuleInterface(std::string moduleName);

    virtual ~ModuleInterface();

    virtual void execute() = 0;
    virtual void shutdown() = 0;
};

class ModuleFactory
{
    std::unordered_map<std::string, std::function<std::shared_ptr<ModuleInterface>(void)>> m_modules;
    std::unordered_map<std::string, std::shared_ptr<ModuleInterface>> loadedModuleClasses;

public:
    std::shared_ptr<ModuleInterface> createModule(const std::string& moduleName);
    void registerModule(const std::string& moduleName, std::function<std::shared_ptr<ModuleInterface>(void)> createFunc);
    void releaseModule(const std::string& moduleName);
    void release();
};

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
    void releaseModuleLib(const std::string& moduleName);
    void releaseModuleClass(const std::string& moduleName);

    // Get the actual path of a loaded module
    std::string getModulePath(const std::string& moduleName);

    std::shared_ptr<ModuleInterface> getModuleClass(const std::string& moduleName);

    static ModuleManager* getInstance();

    void genNewSession();
    long long getSSID() const;

    // Destructor to ensure all modules are released
    ~ModuleManager();
    ModuleManager() = default;

    ModuleFactory Factory;

private:
    static std::unique_ptr<ModuleManager> m_instance;
    static std::once_flag m_flag;
    std::unordered_map<std::string, std::string> modulePaths;
    std::unordered_map<std::string, LibraryHandle> loadedModuleLibs;
    long long _SSID;
};

template <typename T>
class ModuleRegistar
{
public:
    ModuleRegistar(std::string moduleName)
    {
        ModuleManager::getInstance()->Factory.registerModule(moduleName, []() -> std::shared_ptr<ModuleInterface> {
            return std::make_shared<T>();
        });
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
    static const ModuleRegistar<moduleclass> register##moduleclass(moduleName);

#endif // __MODULE_MANAGER_H__
