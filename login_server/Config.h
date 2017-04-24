#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace tinyxml2 { class XMLNode; }

namespace config {

struct ServerConf
{
    std::string     m_path{};           //ÔËÐÐÂ·¾¶
    std::string     m_ip{};
    uint16_t        m_port{};
    size_t          m_client_timeout{}; //seconds 0:never timeout
    size_t          m_max_count{};      //0: unlimited
    std::string     m_script{};
    uint16_t        m_http_port{};
};

struct DBPool
{
    std::string     m_id{};
    std::string     m_name{};
    std::string     m_ip{};
    std::string     m_user{};
    std::string     m_password{};
    uint16_t        m_port{};
    std::string     m_charset{};

    size_t          m_init_size;
    size_t          m_max_size;
};
using DBPoolPtr = std::shared_ptr<DBPool>;

struct DataBaseConf
{
    std::map<std::string, DBPoolPtr> m_pools{};
};

struct RedisNode
{
    std::string     m_id{};
    std::string     m_ip{};
    uint16_t        m_port{};
    std::map<std::string, std::string> m_files;
};
using RedisNodePtr = std::shared_ptr<RedisNode>;

struct RedisConf
{
    RedisNodePtr findNode(const std::string& id) const;

    std::map<std::string, RedisNodePtr> m_redis{};
};

class Config 
{
    Config();
public:
    ~Config();
    static Config& instance();

    bool init();

    const ServerConf* getServerConf() const;
    const RedisConf* getRedisConf() const;
    const DataBaseConf* getDataBaseConf() const;
private:
    bool loadServer(const tinyxml2::XMLNode& node);
    bool loadDataBase(const tinyxml2::XMLNode& node);
    bool loadRedis(const tinyxml2::XMLNode& node);
    //bool loadRedisScript(const boost::property_tree::ptree& pt);
private:
    std::shared_ptr<ServerConf>     m_server_conf;
    std::shared_ptr<DataBaseConf>   m_database_conf;
    std::shared_ptr<RedisConf>      m_redis_conf;
};

inline 
Config& instance() { return Config::instance(); }

}

#endif
