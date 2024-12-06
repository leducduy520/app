#include "main.hpp"
#include "utilities.hpp"
#include "cpprest/asyncrt_utils.h"
#include <boost/predef.h>    // For platform and architecture detection
#include <boost/version.hpp> // For Boost version
#include <thread>            // For hardware concurrency

using namespace dld;

struct MongoData
{
    std::optional<std::string> ca_path;
    std::optional<std::string> db_uri;
    std::optional<std::string> db_name;
    std::optional<std::string> db_coll_name;
};

bool parse_mongo_data(MongoData& data, std::unordered_map<std::string, std::string>& options)
{
    // Check and process the --cert option
    if (options.find("--cert") != options.end())
    {
        data.ca_path = options["--cert"];
        if (std::filesystem::exists(data.ca_path.value()))
        {
            std::cout << "Certificate file provided: " << data.ca_path.value() << "\n";
        }
        else
        {
            std::cerr << "Error: Certificate file not found at " << data.ca_path.value() << "\n";
            return false;
        }
    }
    else
    {
        std::cout << "No certificate file provided. Proceeding without it.\n";
    }

    // Check and process the --db-uri option
    if (options.find("--db-uri") != options.end())
    {
        data.db_uri = options["--db-uri"];
        std::cout << "Database uri provided\n";
    }
    else
    {
        std::cout << "No database uri provided. Proceeding without database uri.\n";
    }

    // Check and process the --db-name option
    if (options.find("--db-name") != options.end())
    {
        data.db_name = options["--db-name"];
        std::cout << "Database name provided: " << data.db_name.value() << "\n";
    }
    else
    {
        std::cout << "No database name provided. Proceeding without database name.\n";
    }

    // Check and process the --db-coll-name option
    if (options.find("--db-coll-name") != options.end())
    {
        data.db_coll_name = options["--db-coll-name"];
        std::cout << "Collection name provided: " << data.db_coll_name.value() << "\n";
    }
    else
    {
        std::cout << "No collection name provided. Proceeding without collection name.\n";
    }
    return true;
}

int main(int argc, char* argv[])
{
    init();
    INDEBUG(printSystemInfo())

    // Parse command line arguments
    auto options = parse_arguments(argc, argv);

    // Parse MongoDB data
    MongoData data;
    parse_mongo_data(data, options);


    try
    {
        // Connect to the database
        DBINSTANCE->Connect(data.db_uri.value_or(dld::get_database_uri().value_or("")), data.ca_path.value_or(""));
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }

    genNewSession(data.db_name.has_value() ? data.db_name : dld::get_database_name(),
                  data.db_coll_name.has_value() ? data.db_coll_name : dld::get_database_collection_name());

    std::string task;
    while (true)
    {
        ask_for_task(task);
        auto etask = magic_enum::enum_cast<ModuleName>(task);
        if (etask.has_value())
        {
            std::shared_ptr<ModuleInterface> mdinterface = nullptr;
            std::weak_ptr<ModuleInterface> winterface;
            auto moduleid = toModuleId(etask.value());
            switch (etask.value())
            {
            case ModuleName::GAMES:
            case ModuleName::CALCULATOR:
            case ModuleName::TRANSLATE:
            {
                if (ModuleManager::getInstance()->loadModule(moduleid))
                {
                    mdinterface = ModuleManager::getInstance()->getModuleClass(moduleid);
                    winterface = mdinterface;
                }
            }
            break;
            case ModuleName::REMOVE:
            {
                std::cout << "Type the name of the module you want to remove: ";
                std::cin >> moduleid;
                ModuleManager::getInstance()->releaseModuleInstance(moduleid);
            }
            break;
            default:
                break;
            }
            if (auto minterface = winterface.lock())
            {
                start_module_execution(moduleid);
                minterface->execute();
                end_module_execution(moduleid);
            }
        }
        else if (task == "QUIT")
        {
            break;
        }
        else
        {
            std::cerr << "Invalid task entered\n";
        }
    }
    ModuleManager::getInstance()->endSession();
    std::cout << "Program finished\n";
    return 0;
}

void ask_for_task(std::string& task)
{
    std::cout << "Please enter the task you want to run:\n";
    std::cout << "Available tasks:\n";
    for (const auto& value : magic_enum::enum_values<ModuleName>())
    {
        std::string mid{magic_enum::enum_name(value)};
        tolower_str(mid);
        std::cout << "  -  " << mid << '\n';
    }
    std::cin >> task;
    toupper_str(task);
}

std::unordered_map<std::string, std::string> parse_arguments(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> options;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "--cert" || arg == "--db-uri" || arg == "--db-name" || arg == "--db-coll-name")
        {
            if (i + 1 < argc)
            {
                options[arg] = argv[i + 1];
                ++i; // Skip the next argument since it's the value of this option
            }
            else
            {
                std::cerr << "Error: Missing value for option " << arg << ".\n";
                exit(1);
            }
        }
        else
        {
            std::cerr << "Warning: Unrecognized option " << arg << ".\n";
        }
    }

    return options;
}

void printout_uids()
{
    try
    {
        mongocxx::v_noabi::options::aggregate opts;
        opts.allow_disk_use(true);
        opts.max_time(std::chrono::milliseconds{60000});

        mongocxx::v_noabi::pipeline pipeline;
        pipeline.group(
            make_document(kvp("_id", bsoncxx::types::b_null{}), kvp("uids", make_document(kvp("$addToSet", "$_id")))));
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

void genNewSession(const std::optional<std::string>& db_name, const std::optional<std::string>& db_coll_name)
{
    DBINSTANCE->GetDatabase(db_name.value_or("guest"));
    DBINSTANCE->GetCollection(db_coll_name.value_or("guest"));

    printout_uids();

    std::cout << "Please enter the user name: ";
    std::cin >> uid;

    auto res = DBINSTANCE->GetDocument(make_document(kvp("_id", uid)));
    if (res)
    {
        // dld::print_3type_json_str(res.value().view(), true);
        std::cout << "User " << uid << " is already exists\n";
        DBINSTANCE->UpdateDocument(
            make_document(kvp("_id", uid)),
            make_document(
                kvp("$push",
                    make_document(kvp("history", make_document(kvp("sid", ssid), kvp("modules", make_array())))))));
    }
    else
    {
        std::cout << "Add a new user " << uid << '\n';

        DBINSTANCE->InsertDocument(
            make_document(kvp("_id", uid),
                          kvp("history", make_array(make_document(kvp("sid", ssid), kvp("modules", make_array()))))));
        printout_uids();
    }
}

void start_module_execution(const std::string& module_name)
{
    const std::string module_path = ModuleManager::getInstance()->getModulePath(module_name);
    DBINSTANCE->UpdateDocument(
        make_document(kvp("_id", uid), kvp("history.sid", ssid)),
        make_document(
            kvp("$push",
                make_document(
                    kvp("history.$.modules",
                        make_document(kvp("module_id", module_name),
                                      kvp("module_path", module_path),
                                      kvp("execution_start",
                                          utility::conversions::to_utf8string(
                                              utility::datetime::utc_now().to_string(utility::datetime::RFC_1123))),
                                      kvp("execution_end", "")))))));
}

void end_module_execution(const std::string& module_name)
{
    mongocxx::v_noabi::options::update udp;
    udp.array_filters(
        make_array(make_document(kvp("outer.sid", ssid)), make_document(kvp("inner.module_id", module_name))));
    DBINSTANCE->UpdateDocument(
        make_document(kvp("_id", uid), kvp("history.modules.module_id", module_name)),
        make_document(kvp("$set",
                          make_document(kvp("history.$[outer].modules.$[inner].execution_end",
                                            utility::conversions::to_utf8string(utility::datetime::utc_now().to_string(
                                                utility::datetime::RFC_1123)))))),
        udp);
}

void printSystemInfo()
{

    std::cout << "===== System Information =====\n";

    // Operating system
    std::cout << "Operating System: ";
#if BOOST_OS_WINDOWS
    std::cout << "Windows";
#elif BOOST_OS_LINUX
    std::cout << "Linux";
#elif BOOST_OS_MACOS
    std::cout << "macOS";
#elif BOOST_OS_UNIX
    std::cout << "Unix";
#else
    std::cout << "Unknown";
#endif
    std::cout << '\n';

    // Architecture
    std::cout << "Processor Architecture: ";
#if BOOST_ARCH_X86_64
    std::cout << "x86_64";
#elif BOOST_ARCH_ARM
    std::cout << "ARM";
#elif BOOST_ARCH_PPC
    std::cout << "PowerPC";
#elif BOOST_ARCH_MIPS
    std::cout << "MIPS";
#else
    std::cout << "Unknown";
#endif
    std::cout << '\n';

    // Compiler
    std::cout << "Compiler: ";
#if BOOST_COMP_MSVC
    std::cout << "MSVC";
#elif BOOST_COMP_GNUC
    std::cout << "GCC";
#elif BOOST_COMP_CLANG
    std::cout << "Clang";
#else
    std::cout << "Unknown";
#endif
    std::cout << '\n';

    // Number of CPU cores
    std::cout << "CPU Cores: " << std::thread::hardware_concurrency() << '\n';

    // Boost library version
    std::cout << "Boost Library Version: " << BOOST_LIB_VERSION << '\n';

    std::cout << "==============================\n";
}
