#ifndef __MODULE_MANAGER_H__
#define __MODULE_MANAGER_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

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
    std::string m_modulePath;

public:
    ModuleInterface(const std::string& moduleName, const std::string& modulePath) : m_moduleName(moduleName), m_modulePath(modulePath)
    {
#if defined(_WIN32)
        m_modulePath.append(".dll");
#else
        m_modulePath.append(".so");
#endif
        std::cout << "Loading module " << m_modulePath << " with ID " << m_moduleName << std::endl;
        ModuleManager::getInstance()->registerModule(m_moduleName, m_modulePath);
        if (!ModuleManager::getInstance()->loadModule(m_moduleName))
        {
            throw std::bad_alloc();
        }
    }

    virtual ~ModuleInterface()
    {
        ModuleManager::getInstance()->releaseModule(m_moduleName);
    }

    virtual void execute() = 0;
};

#endif // __MODULE_MANAGER_H__
