#include <bsoncxx/json.hpp>
#include <nlohmann/json.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <iostream>
#include <string>

#if !defined(NDEBUG)
    #define INDEBUG(func) func;
#else
    #define INDEBUG(func) ;
#endif

#define INDEBUG_IF(func, expr) if(expr) {INDEBUG(func)} else {func;};

namespace dld
{
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
} // namespace dld
