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

class DBClient
{
    mongocxx::client m_dbclient;
    const mongocxx::instance m_dbinstance;
    mongocxx::database m_dbdatabase;
    mongocxx::collection m_dbcollection;
    static std::unique_ptr<DBClient> m_instance;
    static std::once_flag m_flag;

public:
    DBClient();
    static DBClient* GetInstance();
    static void DestroyInstance();
    void GetDatabase(const std::string& name);
    void GetCollection(const std::string& name);
    void CreateCollection(const std::string& collectionName);
    bsoncxx::stdx::optional<mongocxx::v_noabi::result::insert_one> InsertDocument(
        const bsoncxx::document::value& document,
        const std::string& collectionName = {});
    bsoncxx::stdx::optional<mongocxx::v_noabi::result::update> UpdateDocument(
        const bsoncxx::v_noabi::document::value& filter,
        const bsoncxx::v_noabi::document::value& update,
        const mongocxx::v_noabi::options::update& options = mongocxx::v_noabi::options::update(),
        const std::string& collectionName = {});
    bsoncxx::stdx::optional<bsoncxx::v_noabi::document::value> GetDocument(
        const bsoncxx::v_noabi::document::value& filter,
        const std::string& collectionName = {});
    bsoncxx::stdx::optional<mongocxx::v_noabi::result::delete_result> DeleteDocument(
        const bsoncxx::v_noabi::document::value& filter,
        const std::string& collectionName = {});
    mongocxx::v_noabi::cursor RunPipeLine(const mongocxx::pipeline& pl,
                                          const mongocxx::options::aggregate& opts,
                                          const std::string& collectionName = {});
};

#endif /*  __DBCLIENT_GAME__ */
