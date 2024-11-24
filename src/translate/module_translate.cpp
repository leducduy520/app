#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include "module_translate.hpp"
#include "thread_pool.hpp"
#include "utilities.hpp"
#include <cstdlib>
#include <locale>
#include <codecvt>

using namespace utility;              // Common utilities like string conversions
using namespace web;                  // Common features like URIs.
using namespace web::http;            // Common HTTP functionality
using namespace web::http::client;    // HTTP client features
using namespace concurrency::streams; // Asynchronous streams

// using namespace std;
REGISTER_MODULE_CLASS(ModuleTranslator, NAME)

string_t cast_wstring_1(const char* cstr)
{
#ifdef _UTF16_STRINGS
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(cstr);
#else
    return string_t(cstr);
#endif
}

string_t cast_wstring_2(const char* cstr)
{
#ifdef _UTF16_STRINGS
    const size_t len = std::mbstowcs(nullptr, cstr, 0) + 1; // Get the required length

    // Allocate buffer for the wide string
    wchar_t* wbuffer = new wchar_t[len];
    std::mbstowcs(wbuffer, cstr, len);

    // Convert to std::wstring
    std::wstring wstr(wbuffer);
    delete[] wbuffer; // Free the buffer
    return wstr;
#else
    return string_t(cstr);
#endif
}

ModuleTranslator::ModuleTranslator() : ModuleInterface(NAME)
{}

void ModuleTranslator::execute()
{
    // std::locale::global(std::locale("en_US.UTF-8"));
    // std::wcout.imbue(std::locale());

    const char* rapidapi_key = std::getenv("RAPIDAPI_KEY");
    if (rapidapi_key == nullptr)
    {
        std::cerr << "RAPIDAPI_KEY environment variable is not set.\n";
        return;
    }
    m_apiKey = cast_wstring_1(rapidapi_key);

    this->get_language_list();
    auto requestTask = this->do_translate();

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        requestTask.wait();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error exception:" << e.what() << '\n';
    }
}

void ModuleTranslator::shutdown()
{
    std::cout << m_moduleName << " translation finished\n";
}

void ModuleTranslator::get_language_list()
{
    const uri url(U("https://deep-translate1.p.rapidapi.com/language/translate/v2/languages"));
    http_client client(url);

    http_request request_lang_list(methods::GET);
    request_lang_list.headers().set_content_type(U("application/json"));
    request_lang_list.headers().add(U("x-rapidapi-key"), m_apiKey);
    request_lang_list.headers().add(U("x-rapidapi-host"), U("deep-translate1.p.rapidapi.com"));

    auto requestTask = client
                           .request(request_lang_list)

                           // Handle response headers arriving.
                           .then([=](const http_response& response) {
                               if (response.status_code() == status_codes::OK)
                               {
                                   return response.extract_json();
                               }
                               else
                               {
                                   throw std::runtime_error("Failed to get language list");
                               }
                           })
                           .then([](json::value response_data) {
                               auto lang_list = response_data["languages"].as_array();
                               const auto impl = [](web::json::value lang) {
                                   const string_t sense = lang.at(U("language")).as_string() + U(" (") +
                                                          lang.at(U("name")).as_string() + U(")");
                                   ucout << std::left;
                                   ucout << std::setw(20) << sense;
                                   return;
                               };
                                dld::print_in_columns(lang_list, 5, impl);
                               return;
                           });

    try
    {
        requestTask.wait();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // Handle response body arriving.
}

pplx::task<void> ModuleTranslator::do_translate()
{
    string_t text{};
    string_t source_lang{};
    string_t target_lang{};

    std::cout << "Enter the source language (e.g., en, vi, fr, de, etc.):\n";
    ucin >> source_lang;

    std::cout << "Enter the target language (e.g , en, vi, fr, de, etc.)\n";
    ucin >> target_lang;

    std::cout << "Enter the string in English you want to translate to Vietnamese:\n";
    ucin.clear();
    ucin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(ucin, text);

    if (text == U("quit"))
    {
        std::cout << "Goodbye!\n";
        return pplx::task<void>{};
    }

    const uri url(U("https://deep-translate1.p.rapidapi.com/language/translate/v2"));
    http_client client(url);

    json::value request_data;
    request_data[U("q")] = json::value::string(text);
    request_data[U("source")] = json::value::string(source_lang);
    request_data[U("target")] = json::value::string(target_lang);

    http_request request_translate(methods::POST);
    request_translate.headers().set_content_type(U("application/json"));
    request_translate.headers().add(U("x-rapidapi-key"), m_apiKey);
    request_translate.headers().add(U("x-rapidapi-host"), U("deep-translate1.p.rapidapi.com"));
    request_translate.set_body(request_data);

    return client
        .request(request_translate)

        // Handle response headers arriving.
        .then([=](const http_response& response) {
            if (response.status_code() == status_codes::OK)
            {
                return response.extract_json();
            }
            throw std::runtime_error("Request failed with status code " + std::to_string(response.status_code()));
        })
        .then([](json::value response_data) {
            ucout << U("Translation: ") << response_data[U("data")][U("translations")][U("translatedText")].as_string()
                  << '\n';
            return;
        });
}
