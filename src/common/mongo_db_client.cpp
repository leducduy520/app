#include "mongo_db_client.hpp"
#include <cstdlib>

std::unique_ptr<DBClient> DBClient::m_instance;
std::once_flag DBClient::m_flag;

bsoncxx::stdx::optional<string> get_database_uri()
{
    const char* database_uri = std::getenv("MONGODB_URI");
    if (database_uri != nullptr)
    {
        return database_uri;
    }
    return {};
}

bsoncxx::stdx::optional<string> get_database_name()
{
    const char* database_name = std::getenv("MONGODB_NAME");
    if (database_name != nullptr)
    {
        return database_name;
    }
    return {};
}

bsoncxx::stdx::optional<string> get_coll_name()
{
    const char* coll_name = std::getenv("MONGODB_COLL");
    if (coll_name != nullptr)
    {
        return coll_name;
    }
    return {};
}

DBClient::DBClient()
{
    auto str_uri = get_database_uri();
    if (str_uri)
    {
        const auto uri = mongocxx::uri{str_uri.value().c_str()};
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);
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

const mongocxx::database* DBClient::GetDatabase(const char* name)
{
    m_dbdatabase = m_dbclient[name];
    return &m_dbdatabase;
}

const mongocxx::collection* DBClient::GetCollection(const char* name)
{
    if (m_dbdatabase.has_collection(name))
    {
        m_dbcollection = m_dbdatabase[name];
        return &m_dbcollection;
    }
    return nullptr;
}

bool DBClient::CreateCollection(const std::string& collectionName, mongocxx::database* db)
{
    if (db == nullptr)
    {
        m_dbcollection = m_dbdatabase.create_collection(collectionName);
        return true;
    }
    auto databases = m_dbclient.list_database_names();
    if (std::find(databases.begin(), databases.end(), db->name()) != databases.end())
    {
        db->create_collection(collectionName);
        m_dbcollection = db->collection(collectionName);
        return true;
    }
    return false;
}

bool DBClient::InsertDocument(const bsoncxx::document::value& document, mongocxx::collection* collection)
{
    if (collection == nullptr)
    {
        m_dbcollection.insert_one(document.view());
        return true;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        collection->insert_one(document.view());
        return true;
    }
    return false;
}

bool DBClient::UpdateDocument(const bsoncxx::v_noabi::document::value& filter,
                              const bsoncxx::v_noabi::document::value& update,
                              mongocxx::collection* collection)
{
    if (collection == nullptr)
    {

        m_dbcollection.update_one(filter.view(), update.view());
        return true;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        collection->update_one(filter.view(), update.view());
        return true;
    }
    return false;
}

optional<bsoncxx::v_noabi::document::value> DBClient::GetDocument(const bsoncxx::v_noabi::document::value& filter,
                                                                  mongocxx::collection* collection)
{
    if (collection == nullptr)
    {
        return m_dbcollection.find_one(filter.view());
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        return collection->find_one(filter.view());
    }
    return {};
}

bool DBClient::DeleteDocument(const bsoncxx::v_noabi::document::value& filter, mongocxx::collection* collection)
{
    if (collection == nullptr)
    {
        auto result = m_dbcollection.delete_one(filter.view());
        return result.has_value();
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        auto result = collection->delete_one(filter.view());
        if (result)
        {
            return true;
        }
    }
    return false;
}

void DBClient::RunPipeLine(const mongocxx::pipeline& pipeline,
                           const mongocxx::options::aggregate& opts,
                           mongocxx::collection* collection)
{
    if (collection == nullptr)
    {
        auto cursor = m_dbcollection.aggregate(pipeline, opts);
        auto temp = std::distance(cursor.begin(), cursor.end());
        return;
    }
    if (m_dbdatabase.has_collection(collection->name()))
    {
        auto cursor = collection->aggregate(pipeline, opts);
        auto temp = std::distance(cursor.begin(), cursor.end());
        //std::cout << std::distance(cursor.begin(), cursor.end()) << '\n';
    }
}

// void DBClient::testFunc()
// {
//     using bsoncxx::builder::stream::close_array;
//     using bsoncxx::builder::stream::close_document;
//     using bsoncxx::builder::stream::open_array;
//     using bsoncxx::builder::stream::open_document;
// }
