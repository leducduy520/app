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
    {

    }
    void TearDown() override
    {

    }
};

TEST_F(ApiTestCase, MongoEnvVariables)
{
    EXPECT_TRUE(dld::get_database_uri().has_value());
    EXPECT_TRUE(dld::get_database_name().has_value());
    EXPECT_TRUE(dld::get_database_collection_name().has_value());
}


TEST_F(ApiTestCase, ConnectToMongoDatabase)
{
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->Connect("mongodb://root:example@localhost:27017"));
}

 TEST_F(ApiTestCase, APIService)
 {
     using namespace dld::record;
     RecordRequest request;
     {
         const web::http::uri url(
             utility::conversions::to_string_t(std::string("http://localhost:3000").append("/records")));
         web::http::client::http_client client(url);

         web::http::http_request a_request(web::http::methods::GET);

         auto task = client.request(a_request).then([=](const web::http::http_response& response) {
             if (response.status_code() == web::http::status_codes::OK)
             {
                 return response.extract_json();
             }
             throw std::runtime_error(std::string("A request failed with status code ") + std::to_string(response.status_code()));
         });

         EXPECT_NO_THROW(task.wait());
     }

     {
         dld::SimpleRestfulAPI simple_request;
         auto task = simple_request.get(
             utility::conversions::to_string_t(std::string("http://localhost:3000").append("/records")));
         EXPECT_NO_THROW(task.wait());
     }

     {
         dld::record::RecordRequest record_request;
         auto task = record_request.get(
             dld::record::uri_builder(
                 utility::conversions::to_string_t(std::string("http://localhost:3000")))
                 .records());
         EXPECT_NO_THROW(task.wait());
     }
 }
