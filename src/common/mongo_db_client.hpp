#ifndef __DBCLIENT_GAME__
#define __DBCLIENT_GAME__

#define DBINSTANCE DBClient::GetInstance()

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>
#include <mutex>
#include <string>
#include <memory>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

bsoncxx::stdx::optional<std::string> get_database_uri();
bsoncxx::stdx::optional<std::string> get_database_name();
bsoncxx::stdx::optional<std::string> get_coll_name();

class DBClient
{
    mongocxx::client m_dbclient;
    mongocxx::instance m_dbinstance;
    mongocxx::database m_dbdatabase;
    mongocxx::collection m_dbcollection;
    static std::unique_ptr<DBClient> m_instance;
    static std::once_flag m_flag;

public:
    DBClient();
    static DBClient* GetInstance();
    static void DestroyInstance();
    void GetDatabase(const char* name);
    void GetCollection(const char* name);
    void CreateCollection(const std::string& collectionName);
    bool InsertDocument(const bsoncxx::document::value& document, mongocxx::collection* collection = nullptr);
    bool UpdateDocument(const bsoncxx::v_noabi::document::value& filter,
                        const bsoncxx::v_noabi::document::value& update,
                        const mongocxx::v_noabi::options::update& options = mongocxx::v_noabi::options::update(),
                        mongocxx::collection* collection = nullptr);
    bsoncxx::stdx::optional<bsoncxx::v_noabi::document::value> GetDocument(
        const bsoncxx::v_noabi::document::value& filter,
        mongocxx::collection* collection = nullptr);
    bool DeleteDocument(const bsoncxx::v_noabi::document::value& filter, mongocxx::collection* collection = nullptr);
    mongocxx::v_noabi::cursor RunPipeLine(const mongocxx::pipeline& pl,
                     const mongocxx::options::aggregate& opts,
                     mongocxx::collection* collection = nullptr);
};

#endif /*  __DBCLIENT_GAME__ */
