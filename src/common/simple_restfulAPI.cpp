#include "simple_restfulAPI.hpp"
#include <string>

using namespace utility;              // Common utilities like string conversions
using namespace web;                  // Common features like URIs.
using namespace web::http;            // Common HTTP functionality
using namespace web::http::client;    // HTTP client features
using namespace concurrency::streams; // Asynchronous streams
using namespace std::literals::string_literals;

namespace dld
{
    SimpleRestfulAPI::SimpleRestfulAPI(const utility::string_t& a_api_host) : m_api_host(a_api_host)
    {}

    void SimpleRestfulAPI::setApi_host(const utility::string_t& a_api_host)
    {
        m_api_host = a_api_host;
    }

    void SimpleRestfulAPI::setApi_key(const utility::string_t& a_api_key)
    {
        m_api_key = a_api_key;
    }

    pplx::task<web::json::value> SimpleRestfulAPI::get(const utility::string_t& x_url)
    {
        const uri url(x_url);
        http_client client(x_url);

        http_request a_request(methods::GET);
        a_request.headers().set_content_type(U("application/json"));
        a_request.headers().add(U("x-rapidapi-key"), m_api_key);
        a_request.headers().add(U("x-rapidapi-host"), m_api_host);

        return client
            .request(a_request)

            // Handle response headers arriving.
            .then([=](const http_response& response) {
                if (response.status_code() == status_codes::OK)
                {
                    return response.extract_json();
                }
                throw std::runtime_error("A request failed with status code "s +
                                         std::to_string(response.status_code()));
            });
    }

    pplx::task<web::json::value> SimpleRestfulAPI::post(const utility::string_t& x_url, const web::json::value& body)
    {
        const uri url(x_url);
        http_client client(x_url);

        http_request a_request(methods::POST);
        a_request.headers().set_content_type(U("application/json"));
        a_request.headers().add(U("x-rapidapi-key"), m_api_key);
        a_request.headers().add(U("x-rapidapi-host"), m_api_host);
        a_request.set_body(body);

        return client
            .request(a_request)

            // Handle response headers arriving.
            .then([=](const http_response& response) {
                if (response.status_code() == status_codes::OK)
                {
                    return response.extract_json();
                }
                throw std::runtime_error("A request failed with status code "s +
                                         std::to_string(response.status_code()));
            });
    }

    RapidApiRequest::RapidApiRequest() : SimpleRestfulAPI(U("deep-translate1.p.rapidapi.com"))
    {}
} // namespace dld
