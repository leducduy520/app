#ifndef __MODULE_MANAGER_H__
#define __MODULE_MANAGER_H__

#include <iostream>
#include <string>
#include <unordered_map>

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
    FunctionAddress getFunction(const std::string& moduleName, const std::string& functionName);

    // Release a loaded module
    void releaseModule(const std::string& moduleName);

    // Get the actual path of a loaded module
    std::string getModulePath(const std::string& moduleName);

    // Destructor to ensure all modules are released
    ~ModuleManager();

private:
    std::unordered_map<std::string, std::string> modulePaths;
    std::unordered_map<std::string, LibraryHandle> loadedModules;
};

#endif // __MODULE_MANAGER_H__
