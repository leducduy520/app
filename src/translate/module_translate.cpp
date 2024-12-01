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

    while (true) {
        string_t task{};
        ucout << "Enter a task you want me to execute: :\n"
              << "\t- translate: translate the source language to the target language\n"
              << "\t- language: print available languages\n"
              << "\t- detect: detect a input language\n"
              << "\t- exit: quit the program\n";
        ucin >> task;
        if (task == U("translate"))
        {
            this->do_translate();
        }
        else if (task == U("language"))
        {
            this->do_print_available_language();
        }
        else if (task == U("detect"))
        {
            this->do_detect();
        }
        else if (task == U("exit"))
        {
            break;
        }
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
                               throw std::runtime_error("Failed to get language list");
                           })
                           .then([](json::value response_data) {
                               auto lang_list = response_data["languages"].as_array();
                               std::unordered_map<string_t, string_t> languages;
                               for (const auto& lang : lang_list)
                               {
                                   languages[lang.at(U("language")).as_string()] = lang.at(U("name")).as_string();
                               }
                               return languages;
                           });

    try
    {
        m_language_map = requestTask.get();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // Handle response body arriving.
}

void ModuleTranslator::do_translate()
{
    string_t text{};
    string_t source_lang{};
    string_t target_lang{};

    std::cout << "Enter the source language (e.g., en, vi, fr, de, etc.):\n";
    ucin >> source_lang;

    std::cout << "Enter the target language (e.g , en, vi, fr, de, etc.)\n";
    ucin >> target_lang;

    ucout << U("Enter the string in ") << m_language_map[source_lang] << U(" you want to translate to ")
              << m_language_map[target_lang] << U(":\n");
    ucin.clear();
    ucin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(ucin, text);

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

    auto requestTask =
        client
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
                ucout << U("Translation: ")
                      << response_data[U("data")][U("translations")][U("translatedText")].as_string() << '\n';
                return;
            });

    try
    {
        requestTask.wait();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error exception:" << e.what() << '\n';
    }
}

void ModuleTranslator::do_detect()
{
    string_t text{};
    std::cout << "Enter the string you want to detect:\n";
    ucin.clear();
    ucin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(ucin, text);

    const uri url(U("https://deep-translate1.p.rapidapi.com/language/translate/v2/detect"));
    http_client client(url);

    json::value request_data;
    request_data[U("q")] = json::value::string(text);

    http_request request_translate(methods::POST);
    request_translate.headers().set_content_type(U("application/json"));
    request_translate.headers().add(U("x-rapidapi-key"), m_apiKey);
    request_translate.headers().add(U("x-rapidapi-host"), U("deep-translate1.p.rapidapi.com"));
    request_translate.set_body(request_data);

    auto requestTask =
        client
            .request(request_translate)

            // Handle response headers arriving.
            .then([=](const http_response& response) {
                if (response.status_code() == status_codes::OK)
                {
                    return response.extract_json();
                }
                throw std::runtime_error("Request failed with status code " + std::to_string(response.status_code()));
            })
            .then([=](json::value response_data) {
                ucout << U("Detected: ")
                      << m_language_map[response_data[U("data")][U("detections")][0][U("language")].as_string()] << '\n';
                return;
            });

    try
    {
        requestTask.wait();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error exception:" << e.what() << '\n';
    }
}

void ModuleTranslator::do_print_available_language()
{
    std::cout << "Available languages:\n";
    const auto impl = [](std::pair<string_t, string_t> lang_pair) {
        const string_t sense = lang_pair.first + U(" (") + lang_pair.second + U(")");
        ucout << std::left;
        ucout << std::setw(20) << sense;
        return;
    };
    dld::print_in_columns(m_language_map, 5, impl);
}
