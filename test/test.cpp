#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <future>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>
#include "mongo_db_client.hpp"
#include "simple_restfulAPI.hpp"
#include "thread_pool.hpp"
#include "utilities.hpp"
#include "module_manager.h"

class TestCase : public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};

TEST_F(TestCase, Utilities)
{
    std::vector<int> tests{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
    std::function impl = [](const int& a) {
        std::cout << std::setw(5) << a;
        return void(0);
    };
    dld::print_in_columns(tests, 5, impl);

    EXPECT_FALSE(dld::getEnv("ABC").has_value());

    std::string json_str = {"{\"key\":\"value\"}"};
    auto x1 = dld::to_njson(json_str);
    EXPECT_TRUE(x1.is_object());
    EXPECT_TRUE(x1.contains("key"));
    auto d1 = make_document(kvp("key", "value"));
    auto x2 = dld::to_njson(d1.view());
    EXPECT_TRUE(x2.is_object());
    EXPECT_TRUE(x2.contains("key"));
    dld::print_3type_json_str(d1.view());
}

TEST_F(TestCase, ThreadPool)
{
    auto* pool = dld::ThreadPool::getInstance();
    std::vector<std::future<void>> futures(10);

    for (int i = 0; i < 10; ++i)
    {
        futures[i] = (pool->submit(
            100,
            [](unsigned int ms) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                std::cout << "Worker Thread ID: " << std::this_thread::get_id() << '\n';
            },
            100));
    }


    for (auto& fut : futures)
    {
        EXPECT_NO_THROW(fut.get());
    }

    futures.emplace_back(
        pool->submit(100, [](unsigned int ms) { throw std::runtime_error("Thrown when trying to submit"); }, 100));

    EXPECT_THROW(futures.back().get(), std::runtime_error);
}

TEST_F(TestCase, ModuleManager)
{
    auto manager = dld::ModuleManager::getInstance();
    std::string namemodule = "games";
    manager->registerModule(namemodule, "games.so");
    EXPECT_TRUE(manager->loadModule(namemodule));
    auto create = manager->getModuleMethod(namemodule, "createGame");
    EXPECT_TRUE(create!= nullptr);
    // Define a function pointer type matching the signature
    typedef void* (*CreatFunction)(const char *);

    // Cast the symbol to the function pointer
    CreatFunction GameCreate = reinterpret_cast<CreatFunction>(create);
    EXPECT_NO_THROW(auto p = GameCreate("GameA"));
}

TEST_F(TestCase, MongoEnvVariables)
{
    ASSERT_TRUE(dld::get_database_uri().has_value());
    ASSERT_TRUE(dld::get_database_name().has_value());
    ASSERT_TRUE(dld::get_database_collection_name().has_value());
    ASSERT_TRUE(dld::get_api_base_uri().has_value());
}

TEST_F(TestCase, ConnectToMongoDatabase)
{
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->Connect(dld::get_database_uri().value_or("")));
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->GetDatabase(dld::get_database_name().value_or("")));
    EXPECT_NO_THROW(dld::DBClient::GetInstance()->GetCollection(dld::get_database_collection_name().value_or("")));
}

TEST_F(TestCase, APIService)
{
    using namespace dld::record;
    auto base_url = utility::conversions::to_string_t(dld::get_api_base_uri().value_or("http://localhost:3000"));
    ucerr << "api base url: " << base_url << '\n';
    {
        const web::http::uri url(base_url + U("/records"));
        web::http::client::http_client client(url);

        const web::http::http_request a_request(web::http::methods::GET);

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
