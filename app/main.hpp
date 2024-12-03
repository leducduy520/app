#pragma once
#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

#include <string>
#include <iostream>
#include <memory>
#include <locale>
#include <codecvt>
#include <unordered_map>
#include <chrono>

#include "pre-definition.hpp"
#include "mongo_db_client.hpp"

static std::string uid;
static bsoncxx::types::b_oid ssid;

inline void toupper_str(std::string& str)
{
    for (auto& cha : str)
    {
        cha = static_cast<char>(toupper(cha));
    }
}

inline void tolower_str(std::string& str)
{
    for (auto& cha : str)
    {
        cha = static_cast<char>(tolower(cha));
    }
}

inline std::string toModuleId(ModuleName e_moduleName)
{
    auto str = std::string(magic_enum::enum_name(e_moduleName));
    tolower_str(str);
    return str;
}

void printSystemInfo();

void ask_for_task(std::string& task);

std::unordered_map<std::string, std::string> parse_arguments(int argc, char* argv[]);

void printout_uids();

void genNewSession(const std::optional<std::string> &db_name, const std::optional<std::string> &db_coll_name);

void start_module_execution(const std::string& module_name);

void end_module_execution(const std::string& module_name);
