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

#define INDEBUG_IF(func, expr)                                                                                         \
    if (expr)                                                                                                          \
    {                                                                                                                  \
        INDEBUG(func)                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        func;                                                                                                          \
    };

namespace dld
{
    inline nlohmann::json to_njson(const std::string& str)
    {
        return nlohmann::json::parse(str);
    }

    inline nlohmann::json to_njson(const bsoncxx::document::view_or_value& value,
                                   bsoncxx::ExtendedJsonMode mode = bsoncxx::ExtendedJsonMode::k_canonical)
    {
        return nlohmann::json::parse(bsoncxx::to_json(value.view(), mode));
    }

    inline void print_3type_json_str(const bsoncxx::document::view_or_value& value,
                                     const bool debug_only = false) noexcept
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
        if (count % columns != 0)
        {
            std::cout << '\n';
        }
    }

    inline bsoncxx::stdx::optional<std::string> getEnv(const std::string& name)
    {
        const char* value = std::getenv(name.c_str());
        if (value != nullptr)
        {
            return value;
        }
        return {};
    }

    inline bsoncxx::stdx::optional<std::string> get_database_collection_name()
    {
        static auto res = getEnv("MONGODB_COLL");
        return res;
    }

    inline bsoncxx::stdx::optional<std::string> get_database_name()
    {
        static auto res = getEnv("MONGODB_NAME");
        return res;
    }

    inline bsoncxx::stdx::optional<std::string> get_database_uri()
    {
        static auto res = getEnv("MONGODB_URI");
        return res;
    }

    inline bsoncxx::stdx::optional<std::string> get_api_base_uri()
    {
        static auto res = getEnv("API_BASE_URI");
        return res;
    }
} // namespace dld
#endif
