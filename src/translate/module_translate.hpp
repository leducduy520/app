#pragma once

#include "module_manager.h"
#include <cpprest/http_client.h>

class ModuleTranslator : public ModuleInterface
{
public:
    ModuleTranslator();
    void execute() override;
    void shutdown() override;
    void get_language_list();
    pplx::task<void> do_translate();
private:
    utility::string_t m_apiKey;
};
