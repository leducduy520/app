#pragma once

#include "module_manager.h"
#include "simple_restfulAPI.hpp"
#include <cpprest/http_client.h>
#include <unordered_map>

using namespace dld;

class TranslationalRequest : public dld::RapidApiRequest
{
public:
    static pplx::task<web::json::value> get_available_languages();
    static pplx::task<web::json::value> get_translation(const utility::string_t& text, const utility::string_t& src, const utility::string_t& des);
    static pplx::task<web::json::value> get_detection(const utility::string_t& text);
    static RapidApiRequest request;
private:
};

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
    std::unordered_map<utility::string_t, utility::string_t> m_language_map;
};
