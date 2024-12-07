#ifndef __DLD_UTILS__
#define __DLD_UTILS__
#include <bsoncxx/json.hpp>
#include <nlohmann/json.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <iostream>
#include <string>
#include <functional>
#include <cstdlib>

#if !defined(NDEBUG)
    #define INDEBUG(func) func;
#else
    #define INDEBUG(func) ;
#endif

#define INDEBUG_IF(func, expr) if(expr) {INDEBUG(func)} else {func;};

namespace dld
{
    inline nlohmann::json to_njson(const std::string &str)
    {
        return nlohmann::json::parse(str);
    }
    
    inline nlohmann::json to_njson(const bsoncxx::document::view_or_value& value,
                                   bsoncxx::ExtendedJsonMode mode = bsoncxx::ExtendedJsonMode::k_canonical)
    {
        return nlohmann::json::parse(bsoncxx::to_json(value.view(), mode));
    }

    inline void print_3type_json_str(const bsoncxx::document::view_or_value& value, const bool debug_only = false) noexcept
    {
        auto impl = [&value]() {
            std::cout << "Converted nlohmann::json object with mode legacy: \n"
                      << to_njson(value, bsoncxx::ExtendedJsonMode::k_legacy).dump(2) << '\n';
            std::cout << "Converted nlohmann::json object with mode relaxed: \n"
                      << to_njson(value, bsoncxx::ExtendedJsonMode::k_relaxed).dump(2) << '\n';
            std::cout << "Converted nlohmann::json object with mode canonical: \n"
                      << to_njson(value, bsoncxx::ExtendedJsonMode::k_canonical).dump(2) << '\n';
        };

        try
        {
            INDEBUG_IF(impl(), debug_only)
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    template <typename T, typename Y>
    inline void print_in_columns(const T& array, int columns, Y print)
    {
        int count = 0;
        for (const auto& item : array)
        {
            print(item);
            count++;
            if (count % columns == 0)
            {
                std::cout << '\n';
            }
        }
        if (count % columns!= 0)
        {
            std::cout << '\n';
        }
    }
    inline bsoncxx::stdx::optional<std::string> get_database_collection_name()
    {
        const char* coll_name = std::getenv("MONGODB_COLL");
        if (coll_name != nullptr)
        {
            return std::string{coll_name};
        }
        return {};
    }
    inline bsoncxx::stdx::optional<std::string> get_database_name()
    {
        const char* database_name = std::getenv("MONGODB_NAME");
        if (database_name != nullptr)
        {
            return database_name;
        }
        return {};
    }
    inline bsoncxx::stdx::optional<std::string> get_database_uri()
    {
        const char* database_uri = std::getenv("MONGODB_URI");
        if (database_uri != nullptr)
        {
            return database_uri;
        }
        return {};
    }
    inline bsoncxx::stdx::optional<std::string> get_api_base_uri()
    {
        const char* api_base_uri = std::getenv("API_BASE_URI");
        if (api_base_uri != nullptr)
        {
            return api_base_uri;
        }
        return {};
    }
} // namespace dld
#endif
