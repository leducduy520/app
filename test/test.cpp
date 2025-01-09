#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mongo_db_client.hpp"
#include "simple_restfulAPI.hpp"
#include "utilities.hpp"
#include <string>

class ApiTestCase : public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};

TEST_F(ApiTestCase, MongoEnvVariables)
{
    ASSERT_TRUE(dld::get_database_uri().has_value());
    ASSERT_TRUE(dld::get_database_name().has_value());
    ASSERT_TRUE(dld::get_database_collection_name().has_value());
    ASSERT_TRUE(dld::get_api_base_uri().has_value());
}

TEST_F(ApiTestCase, ConnectToMongoDatabase)
{
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->Connect(dld::get_database_uri().value_or("")));
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->GetDatabase(dld::get_database_name().value_or("")));
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->GetCollection(dld::get_database_collection_name().value_or("")));
}

TEST_F(ApiTestCase, APIService)
{
    using namespace dld::record;
    auto base_url = utility::conversions::to_string_t(dld::get_api_base_uri().value_or("http://localhost:3000"));
    ucerr << "api base url: " << base_url << '\n';
    {
        const web::http::uri url(base_url + U("/records"));
        web::http::client::http_client client(url);

        web::http::http_request a_request(web::http::methods::GET);

        auto task = client.request(a_request).then([=](const web::http::http_response& response) {
            if (response.status_code() == web::http::status_codes::OK)
            {
                return response.extract_json();
            }
            throw std::runtime_error(std::string("A request failed with status code ") +
                                     std::to_string(response.status_code()));
        });

        EXPECT_NO_THROW(task.wait());
    }

    {
        dld::SimpleRestfulAPI simple_request;
        auto task = simple_request.get(base_url + U("/records"));
        EXPECT_NO_THROW(task.wait());
    }

    {
        RecordRequest record_request;
        auto task = record_request.get(uri_builder(base_url).records());
        EXPECT_NO_THROW(task.wait());
    }
}
