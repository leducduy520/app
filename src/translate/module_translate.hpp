#pragma once

#include "module_manager.h"
#include <cpprest/http_client.h>
#include <unordered_map>

using namespace dld;

class ModuleTranslator : public ModuleInterface
{
public:
    ModuleTranslator();
    void execute() override;
    void shutdown() override;
    void get_language_list();
    void do_translate();
    void do_detect();
    void do_print_available_language();
private:
    utility::string_t m_apiKey;
    std::unordered_map<utility::string_t, utility::string_t> m_language_map;
};
