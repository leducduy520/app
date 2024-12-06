#ifndef __SIMPLE_RESTFULAPI__
#define __SIMPLE_RESTFULAPI__

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

namespace dld
{
    class SimpleRestfulAPI
    {
    public:
        SimpleRestfulAPI() = default;
        SimpleRestfulAPI(const utility::string_t& a_api_host);
        void setApi_host(const utility::string_t& a_api_host);
        void setApi_key(const utility::string_t& a_api_key);
        pplx::task<web::json::value> get(const utility::string_t& x_url);
        pplx::task<web::json::value> post(const utility::string_t& x_url, const web::json::value& body);

    private:
        utility::string_t m_api_host;
        utility::string_t m_api_key;
    };
    
    class RapidApiRequest : public SimpleRestfulAPI
    {
    public:
        RapidApiRequest();
        void setApi_host(const utility::string_t& a_api_host) = delete;
    };
} // namespace dld

#endif
