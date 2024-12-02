#include "mongo_db_client.hpp"
#include "utilities.hpp"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::stdx::optional;
using bsoncxx::v_noabi::document::value;
using mongocxx::v_noabi::result::delete_result;
using mongocxx::v_noabi::result::insert_one;
using mongocxx::v_noabi::result::update;

std::unique_ptr<DBClient> DBClient::m_instance;
std::once_flag DBClient::m_flag;
std::filesystem::path DBClient::m_ca_path;

DBClient::DBClient()
{
    auto str_uri = dld::get_database_uri();
    if (str_uri.has_value())
    {
        INDEBUG(std::cout << "mongodb uri: " << str_uri.value() << '\n')

        // Define the MongoDB URI
        const mongocxx::uri uri{str_uri.value().c_str()};

        // Set up client options
        mongocxx::options::client client_options;

        if (!m_ca_path.empty())
        {
            // Configure TLS/SSL options
            mongocxx::options::tls tls_options;

            // Provide the single PEM file (client certificate and private key combined)
            tls_options.pem_file(m_ca_path.c_str()); // Replace with the path to your .pem file

            // Apply TLS options to client options
            client_options.tls_opts(tls_options);
        }

        // Optionally configure Server API version (MongoDB 5.x+)
        client_options.server_api_opts(
            mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1});

        // Create the client with URI and options
        m_dbclient = std::move(mongocxx::client{uri, client_options});
        return;
    }
    throw std::runtime_error("Environment variable MONGODB_URI not set");
}

DBClient* DBClient::GetInstance()
{
    std::call_once(m_flag, []() { m_instance = std::make_unique<DBClient>(); });
    return m_instance.get();
}

void DBClient::GetDatabase(const std::string& name)
{
    m_dbdatabase = m_dbclient[name];
}

void DBClient::GetCollection(const std::string& name)
{
    if (m_dbdatabase.has_collection(name))
    {
        m_dbcollection = m_dbdatabase[name];
        return;
    }
    this->CreateCollection(name);
}

void DBClient::CreateCollection(const std::string& collectionName)
{
    m_dbcollection = m_dbdatabase.create_collection(collectionName);
}

optional<insert_one> DBClient::InsertDocument(const bsoncxx::document::value& document,
                                              const std::string& collectionName)
{
    if (!collectionName.empty())
    {
        GetCollection(collectionName);
    }
    return m_dbcollection.insert_one(document.view());
}

optional<update> DBClient::UpdateDocument(const bsoncxx::v_noabi::document::value& filter,
                                          const bsoncxx::v_noabi::document::value& update,
                                          const mongocxx::v_noabi::options::update& options,
                                          const std::string& collectionName)
{
    if (!collectionName.empty())
    {
        GetCollection(collectionName);
    }
    return m_dbcollection.update_one(filter.view(), update.view(), options);
}

optional<value> DBClient::GetDocument(const bsoncxx::v_noabi::document::value& filter,
                                      const std::string& collectionName)
{
    if (!collectionName.empty())
    {
        GetCollection(collectionName);
    }
    return m_dbcollection.find_one(filter.view());
}

optional<delete_result> DBClient::DeleteDocument(const bsoncxx::v_noabi::document::value& filter,
                                                 const std::string& collectionName)
{
    if (!collectionName.empty())
    {
        GetCollection(collectionName);
    }
    return m_dbcollection.delete_one(filter.view());
}

mongocxx::v_noabi::cursor DBClient::RunPipeLine(const mongocxx::pipeline& pipeline,
                                                const mongocxx::options::aggregate& opts,
                                                const std::string& collectionName)
{
    return m_dbcollection.aggregate(pipeline, opts);
}

// void DBClient::testFunc()
// {
//     using bsoncxx::builder::stream::close_array;
//     using bsoncxx::builder::stream::close_document;
//     using bsoncxx::builder::stream::open_array;
//     using bsoncxx::builder::stream::open_document;
// }
