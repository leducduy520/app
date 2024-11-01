#ifndef __DBCLIENT_GAME__
#define __DBCLIENT_GAME__

#define DBINSTANCE DBClient::GetInstance()

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>
#include <mutex>
#include <string>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::stdx::optional;
using namespace std;


bsoncxx::stdx::optional<string> get_database_uri();
bsoncxx::stdx::optional<string> get_database_name();
bsoncxx::stdx::optional<string> get_coll_name();

class DBClient
{
    mongocxx::client m_dbclient;
    mongocxx::instance m_dbinstance;
    mongocxx::database m_dbdatabase;
    mongocxx::collection m_dbcollection;
    static DBClient* m_instance;
    static std::once_flag m_flag;
    DBClient();

public:
    static DBClient* GetInstance();
    static void DestroyInstance();
    const mongocxx::database* GetDatabase(const char* name);
    const mongocxx::collection* GetCollection(const char* name);
    bool CreateCollection(const std::string& collectionName, mongocxx::database* db = nullptr);
    bool InsertDocument(const bsoncxx::document::value& document, mongocxx::collection* collection = nullptr);
    bool UpdateDocument(const bsoncxx::v_noabi::document::value& filter,
                        const bsoncxx::v_noabi::document::value& update,
                        mongocxx::collection* collection = nullptr);
    optional<bsoncxx::v_noabi::document::value> GetDocument(const bsoncxx::v_noabi::document::value& filter,
                                                            mongocxx::collection* collection = nullptr);
    bool DeleteDocument(const bsoncxx::v_noabi::document::value& filter, mongocxx::collection* collection = nullptr);
    void RunPipeLine(const mongocxx::pipeline& pl,
                     const mongocxx::options::aggregate& opts,
                     mongocxx::collection* collection = nullptr);
};

#endif /*  __DBCLIENT_GAME__ */
