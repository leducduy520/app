#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "mongo_db_client.hpp"
#include "simple_restfulAPI.hpp"
#include "utilities.hpp"

TEST_CASE("Test mongo db environment variables", "[mongo-env-variables]")
{
    CHECK(dld::get_database_uri().has_value());
    CHECK(dld::get_database_name().has_value());
    CHECK(dld::get_database_collection_name().has_value());
}

TEST_CASE("Test mongo db connection", "[mongo-connnection]")
{
    CHECK_NOTHROW(dld::DBClient::GetInstance()->Connect(dld::get_database_uri().value_or("")));
}

TEST_CASE("Test mongo db get database and collection", "[mongo-get-db-coll]")
{
    CHECK_NOTHROW(dld::DBClient::GetInstance()->Connect(dld::get_database_uri().value_or("")));
    CHECK_NOTHROW(dld::DBClient::GetInstance()->GetDatabase(dld::get_database_name().value_or("app")));
    CHECK_NOTHROW(dld::DBClient::GetInstance()->GetCollection(dld::get_database_collection_name().value_or("module_app")));
}

TEST_CASE("Test api services", "[api-services]")
{
    using namespace dld::record;
    RecordRequest request;
    SECTION("s1")
    {
        const web::http::uri url(U("http://localhost:3000/records"));
        web::http::client::http_client client(url);

        web::http::http_request a_request(web::http::methods::GET);

        auto task = client.request(a_request).then([=](const web::http::http_response& response) {
            if (response.status_code() == web::http::status_codes::OK)
            {
                return response.extract_json();
            }
            throw std::runtime_error(U("A request failed with status code ") + std::to_string(response.status_code()));
        });

        CHECK_NOTHROW(task.wait());
    }

    SECTION("s2")
    {
        dld::SimpleRestfulAPI simple_request;
        auto task = simple_request.get(U("http://localhost:3000/records"));
        CHECK_NOTHROW(task.wait());
    }

    SECTION("s3")
    {
        dld::record::RecordRequest record_request;
        auto task = record_request.get(dld::record::uri_builder().records());
        CHECK_NOTHROW(task.wait());
    }
}
