#ifndef __SIMPLE_RESTFULAPI__
#define __SIMPLE_RESTFULAPI__

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

namespace dld
{
    class SimpleRestfulAPI
    {
    public:
        SimpleRestfulAPI() = default;
        void setHeaders(const web::http::http_headers& headers);
        pplx::task<web::json::value> get(const utility::string_t& x_url);
        pplx::task<web::json::value> post(const utility::string_t& x_url, const web::json::value& body);
        pplx::task<web::json::value> put(const utility::string_t& x_url, const web::json::value& body);

    protected:
        web::http::http_request m_api_request;

        pplx::task<web::json::value> send_request(web::http::client::http_client& client,
                                                  web::http::http_request& request);
    };

    class RapidApiRequest : public SimpleRestfulAPI
    {
    public:
        RapidApiRequest();
        void setApi_key_value(const utility::string_t& a_api_key_value);
    };

    namespace record
    {
        struct uri_builder
        {
            utility::string_t value;
            uri_builder(const utility::string_t& base_uri = U("http://localhost:3000"));
            uri_builder& records(const utility::string_t& record_id = {});
            uri_builder& history(const utility::string_t& history_id = {});
            uri_builder& history(const bsoncxx::types::b_oid& history_id);
            uri_builder& modules(const utility::string_t& module_id = {});
        };

        class RecordRequest : protected SimpleRestfulAPI
        {
        public:
            using SimpleRestfulAPI::get;
            using SimpleRestfulAPI::setHeaders;
            RecordRequest();
            pplx::task<web::json::value> get(const uri_builder& x_url);
            pplx::task<web::json::value> post(const uri_builder& x_url, const web::json::value& body);
            pplx::task<web::json::value> put(const uri_builder& x_url, const web::json::value& body);
        };
    } // namespace record
} // namespace dld

#endif
