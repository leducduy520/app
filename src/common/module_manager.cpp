#include "mongo_db_client.hpp"
#include "module_manager.h"
#include <chrono>
#include <filesystem>

using namespace std::chrono;

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

void ModuleManager::genNewSession()
{
    DBINSTANCE->GetDatabase("duyld");
    if(DBINSTANCE->GetCollection("module_app") == nullptr)
    {
        (void)DBINSTANCE->CreateCollection("module_app");
    }

    std::string uid;
    std::cout << "Please enter the user name: ";
    std::cin >> uid;

    _SSID = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    auto res = DBINSTANCE->GetDocument(make_document(kvp("uid", uid)));
    if (res)
    {
        std::cout << "User " << uid << " is already exists\n";
        DBINSTANCE->UpdateDocument(
            make_document(kvp("uid", uid)),
            make_document(
                kvp("$push",
                    make_document(kvp("history", make_document(kvp("sid", _SSID), kvp("modules", make_array())))))));
    }
    else
    {
        std::cout << "Add a new user " << uid << '\n';
        
        DBINSTANCE->InsertDocument(
            make_document(kvp("uid", uid),
                          kvp("history", make_array(make_document(kvp("sid", _SSID), kvp("modules", make_array()))))));
    }
}

long long ModuleManager::getSSID() const
{
    return _SSID;
}

ModuleInterface::ModuleInterface(std::string modulename) : m_moduleName(std::move(modulename))
{
    std::cout << "Module " << m_moduleName << '\n';
    const std::filesystem::path module_path{ModuleManager::getInstance()->getModulePath(m_moduleName)};
    DBINSTANCE->UpdateDocument(
        make_document(kvp("history.sid", ModuleManager::getInstance()->getSSID())),
        make_document(
            kvp("$push",
                make_document(
                    kvp("history.$.modules",
                        make_document(kvp("module_id", m_moduleName),
                                      kvp("module_path", module_path.filename().generic_u8string()),
                                      kvp("module_start", bsoncxx::types::b_date{std::chrono::system_clock::now()}),
                                      kvp("module_end", "")))))));
}

ModuleInterface::~ModuleInterface()
{
    std::cout << "~Module " << m_moduleName << '\n';
    mongocxx::v_noabi::options::update udp;
    udp.array_filters(make_array(make_document(kvp("outer.sid", ModuleManager::getInstance()->getSSID())),
                                 make_document(kvp("inner.module_id", m_moduleName))));
    auto current_time = std::chrono::system_clock::now();
    bsoncxx::types::b_date bson_date{current_time};
    DBINSTANCE->UpdateDocument(
        make_document(kvp("history.sid", ModuleManager::getInstance()->getSSID()),
                      kvp("history.modules.module_id", m_moduleName)),
        make_document(kvp("$set", make_document(kvp("history.$[outer].modules.$[inner].module_end", bson_date)))),
        udp);
}

std::shared_ptr<ModuleInterface> ModuleFactory::createModule(const std::string& moduleName)
{
    auto fit = m_modules.find(moduleName);
    if (fit != m_modules.end())
    {
        if(auto search = loadedModuleClasses.find(moduleName); search == loadedModuleClasses.end())
        {
            auto module = fit->second();
            loadedModuleClasses[moduleName] = module;
        }
        return loadedModuleClasses[moduleName];
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
