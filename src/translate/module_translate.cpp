#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include "module_translate.hpp"
#include "thread_pool.hpp"
#include <cstdlib>
#include <locale>
#include <codecvt>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

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
    auto api_key = cast_wstring_1(rapidapi_key);

    string_t text;
    std::cout << "Enter the string in English you want to translate to Vietnamese:\n";
    ucin.clear();
    ucin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(ucin, text);

    if (text == U("quit"))
    {
        std::cout << "Goodbye!\n";
        return;
    }

    const uri url(U("https://deep-translate1.p.rapidapi.com/language/translate/v2"));
    http_client client(url);

    json::value request_data;
    request_data[U("q")] = json::value::string(text);
    request_data[U("source")] = json::value::string(U("en"));
    request_data[U("target")] = json::value::string(U("vi"));

    http_request request(methods::POST);
    request.headers().set_content_type(U("application/json"));
    request.headers().add(U("x-rapidapi-key"), api_key);
    request.set_body(request_data);
    request.headers().add(U("x-rapidapi-host"), U("deep-translate1.p.rapidapi.com"));
    request.set_body(request_data);
    // Open stream to output file.
    const auto requestTask =
        client
            .request(request)

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
