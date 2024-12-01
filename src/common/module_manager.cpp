#include "mongo_db_client.hpp"
#include "module_manager.h"
#include "utilities.hpp"
#include <chrono>
#include <filesystem>

using namespace std::chrono;

std::unique_ptr<ModuleManager> ModuleManager::m_instance = nullptr;
std::once_flag ModuleManager::m_flag;

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
    auto pit = m_module_handle.find(moduleName);
    if (pit != m_module_handle.end())
    {
        return m_factory.createModule(moduleName);
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
    try
    {
        DBINSTANCE->GetDatabase("duyld");
        DBINSTANCE->GetCollection("module_app");

        printout_uids();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    std::string uid{};
    std::cout << "Please enter the user name: ";
    std::cin >> uid;

    m_ssid = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    auto res = DBINSTANCE->GetDocument(make_document(kvp("uid", uid)));
    if (res)
    {
        // dld::print_3type_json_str(res.value().view(), true);
        std::cout << "User " << uid << " is already exists\n";
        DBINSTANCE->UpdateDocument(make_document(kvp("uid", uid)),
                                   make_document(kvp("$push",
                                                     make_document(kvp("history",
                                                                       make_document(kvp("sid", m_ssid.value()),
                                                                                     kvp("modules", make_array())))))));
    }
    else
    {
        std::cout << "Add a new user " << uid << '\n';

        DBINSTANCE->InsertDocument(make_document(
            kvp("uid", uid),
            kvp("history", make_array(make_document(kvp("sid", m_ssid.value()), kvp("modules", make_array()))))));
        printout_uids();
    }
}

void ModuleManager::printout_uids()
{
    try
    {
        mongocxx::v_noabi::options::aggregate opts;
        opts.allow_disk_use(true);
        opts.max_time(std::chrono::milliseconds{60000});

        mongocxx::v_noabi::pipeline pipeline;
        pipeline.group(
            make_document(kvp("_id", bsoncxx::types::b_null{}), kvp("uids", make_document(kvp("$addToSet", "$uid")))));
        auto cursor = DBINSTANCE->RunPipeLine(pipeline, opts);
        std::cout << "List of existing user:\n";
        for (const auto& line : cursor)
        {
            nlohmann::json json_obj = dld::to_njson(line);
            auto uids = json_obj["uids"].get<std::vector<std::string>>();
            for (auto& uid : uids)
            {
                std::cout << "\t- " << uid << "\n";
            }
            std::cout << '\n';
#ifndef NDEBUG
            std::cout << "\n================== Debug start =================\n";
            std::cout << "Converted nlohmann::json object: \n" << json_obj.dump(2) << '\n';
            std::cout << "================== Debug end =================\n";
#endif
        }

#ifndef NDEBUG
        pipeline = mongocxx::v_noabi::pipeline{};
        pipeline.unwind("$history")
            .unwind("$history.modules")
            .group(make_document(kvp("_id", "$history.modules.module_id"), kvp("count", make_document(kvp("$sum", 1)))))
            .sort(make_document(kvp("count", -1)))
            .limit(1);
        {
            auto cursor = DBINSTANCE->RunPipeLine(pipeline, opts);
            std::cout << "Top most frequently used modules:\n";
            for (const auto& line : cursor)
            {
                nlohmann::json json_obj = dld::to_njson(line);
                std::cout << json_obj["_id"].get<std::string>() << '\n';
            }
        }
#endif
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ModuleManager::endSession()
{
    m_factory.release();
}

std::optional<int64_t> ModuleManager::getSSID() const
{
    return m_ssid;
}

ModuleInterface::ModuleInterface(std::string modulename) : m_moduleName(std::move(modulename))
{
    INDEBUG(std::cout << "Module " << m_moduleName << '\n')
    if(!ModuleManager::getInstance()->getSSID().has_value())
    {
        return;
    }
    const std::filesystem::path module_path{ModuleManager::getInstance()->m_module_paths[m_moduleName]};
    DBINSTANCE->UpdateDocument(
        make_document(kvp("history.sid", ModuleManager::getInstance()->getSSID().value())),
        make_document(kvp("$push",
                          make_document(kvp("history.$.modules",
                                            make_document(kvp("module_id", m_moduleName),
                                                          kvp("module_path", module_path.filename().generic_u8string()),
                                                          kvp("module_start",
                                                              bsoncxx::types::b_date{std::chrono::system_clock::now()}),
                                                          kvp("module_end", "")))))));
}

ModuleInterface::~ModuleInterface()
{
    INDEBUG(std::cout << "~Module " << m_moduleName << '\n')
    if(!ModuleManager::getInstance()->getSSID().has_value())
    {
        return;
    }
    mongocxx::v_noabi::options::update udp;
    udp.array_filters(make_array(make_document(kvp("outer.sid", ModuleManager::getInstance()->getSSID().value())),
                                 make_document(kvp("inner.module_id", m_moduleName))));
    auto current_time = std::chrono::system_clock::now();
    bsoncxx::types::b_date bson_date{current_time};
    DBINSTANCE->UpdateDocument(
        make_document(kvp("history.sid", ModuleManager::getInstance()->getSSID().value()),
                      kvp("history.modules.module_id", m_moduleName)),
        make_document(kvp("$set", make_document(kvp("history.$[outer].modules.$[inner].module_end", bson_date)))),
        udp);
}
