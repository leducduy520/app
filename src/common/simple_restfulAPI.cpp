#include "simple_restfulAPI.hpp"
#include "utilities.hpp"
#include <string>

using namespace utility;              // Common utilities like string conversions
using namespace web;                  // Common features like URIs.
using namespace web::http;            // Common HTTP functionality
using namespace web::http::client;    // HTTP client features
using namespace concurrency::streams; // Asynchronous streams
using namespace std::literals::string_literals;

namespace dld
{
    void SimpleRestfulAPI::setHeaders(const http_headers& headers)
    {
        m_api_request.headers() = headers;
        m_api_request.headers().set_content_type(U("application/json"));
    }

    pplx::task<web::json::value> SimpleRestfulAPI::get(const utility::string_t& x_url)
    {
        const uri url(x_url);
        http_client client(x_url);

        m_api_request.set_method(methods::GET);
        // m_api_request.set_body(web::json::value{});

        return client
            .request(m_api_request)

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

        m_api_request.set_method(methods::POST);
        m_api_request.set_body(body);

        return client
            .request(m_api_request)

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

    pplx::task<web::json::value> SimpleRestfulAPI::put(const utility::string_t& x_url, const web::json::value& body)
    {
        const uri url(x_url);
        http_client client(x_url);

        m_api_request.set_method(methods::PUT);
        m_api_request.set_body(body);

        return client
            .request(m_api_request)

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

    pplx::task<web::json::value> SimpleRestfulAPI::send_request(http_client& client, http_request& request)
    {
        return client
            .request(request)

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

    RapidApiRequest::RapidApiRequest()
    {
        m_api_request.headers().add(U("x-rapidapi-host"), U("deep-translate1.p.rapidapi.com"));
        m_api_request.headers().add(U("x-rapidapi-key"), U(""));
    }

    void RapidApiRequest::setApi_key_value(const utility::string_t& a_api_key_value)
    {
        m_api_request.headers()[U("x-rapidapi-key")] = a_api_key_value;
    }

    namespace record
    {
        uri_builder::uri_builder(const utility::string_t& base_uri) : value(base_uri)
        {}

        uri_builder& uri_builder::records(const utility::string_t& record_id)
        {
            this->value.append(U("/records"));
            if (!record_id.empty())
            {
                this->value.append(U("/")) += record_id;
            }
            return *this;
        }

        uri_builder& uri_builder::history(const utility::string_t& history_id)
        {
            this->value.append(U("/history"));
            if (!history_id.empty())
            {
                this->value.append(U("/")) += history_id;
            }
            return *this;
        }

        uri_builder& uri_builder::history(const bsoncxx::types::b_oid& history_id)
        {
            this->value.append(U("/history"));
            this->value.append(U("/")) += utility::conversions::to_string_t(history_id.value.to_string());
            return *this;
        }

        uri_builder& uri_builder::modules(const utility::string_t& module_id)
        {
            this->value.append(U("modules"));
            if (!module_id.empty())
            {
                this->value.append(U("/")) += module_id;
            }
            return *this;
        }

        RecordRequest::RecordRequest()
        {}

        pplx::task<web::json::value> RecordRequest::get(const uri_builder& x_url)
        {
            return SimpleRestfulAPI::get(x_url.value);
        }

        pplx::task<web::json::value> RecordRequest::post(const uri_builder& x_url, const web::json::value& body)
        {
            return SimpleRestfulAPI::post(x_url.value, body);
        }

        pplx::task<web::json::value> RecordRequest::put(const uri_builder& x_url, const web::json::value& body)
        {
            return SimpleRestfulAPI::put(x_url.value, body);
        }

    } // namespace record
} // namespace dld
