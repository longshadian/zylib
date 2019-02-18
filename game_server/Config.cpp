#include "Config.h"

#include "BoostDefine.h"
#include "Log.h"
#include "tinyxml2.h"

namespace config {

RedisNodePtr RedisConf::findNode(const std::string& id) const
{
    auto it = m_redis.find(id);
    if (it == m_redis.end())
        return nullptr;
    return it->second;
}

Config::Config()
    : m_server_conf(nullptr)
    , m_database_conf(nullptr)
    , m_redis_conf(nullptr)
{
}

Config::~Config()
{
}

Config& Config::instance()
{
    static Config _instance;
    return _instance;
}

const ServerConf* Config::getServerConf() const
{
    return m_server_conf ? &*m_server_conf : nullptr;
}

const RedisConf* Config::getRedisConf() const
{
    return m_redis_conf ? &*m_redis_conf : nullptr;

}
const DataBaseConf* Config::getDataBaseConf() const
{
    return m_database_conf ? &*m_database_conf : nullptr;
}

bool Config::init()
{
    /// 目录格式规定
    ///  ./bin/xxx_server
    ///  ./conf/server.xml
    std::string server_path = "./conf/server.xml";
    LOG(INFO) << "start load " << server_path;

    tinyxml2::XMLDocument xml_doc{};
    if (xml_doc.LoadFile(server_path.c_str())) {
        LOG(ERROR) << "load file '" << server_path << "' failed!";
        return false;
    }
    auto* node_xml = xml_doc.FirstChildElement("xml");
    if (!node_xml) {
        LOG(ERROR) << "can't find <xml> node";
        return false;
    }

    if (!loadServer(*node_xml->FirstChildElement("server")))
        return false;
    if (!loadDataBase(*node_xml->FirstChildElement("database")))
        return false;
    if (!loadRedis(*node_xml->FirstChildElement("redis")))
        return false;
    LOG(INFO) << "load xml success";
    return true;
}

bool Config::loadServer(const tinyxml2::XMLNode& node)
{
    try {
        m_server_conf = std::make_shared<ServerConf>();
        m_server_conf->m_path = node.FirstChildElement("path")->GetText();
        m_server_conf->m_ip = node.FirstChildElement("ip")->GetText();
        m_server_conf->m_port = boost::convert<uint16_t>(node.FirstChildElement("port")->GetText()).value();
        m_server_conf->m_client_timeout = boost::convert<size_t>(node.FirstChildElement("client_timeout")->GetText()).value();
        m_server_conf->m_max_count = boost::convert<size_t>(node.FirstChildElement("max_connect")->GetText()).value();
        m_server_conf->m_script = node.FirstChildElement("script")->GetText();
        m_server_conf->m_http_port = boost::convert<uint16_t>(node.FirstChildElement("http_port")->GetText()).value();

        /*
        LOG(DEBUG) << "<server>";
        LOG(DEBUG) << "path:" << m_server_conf->m_path;
        LOG(DEBUG) << "ip:" << m_server_conf->m_ip;
        LOG(DEBUG) << "port:" << m_server_conf->m_port;
        LOG(DEBUG) << "client_timeout:" << m_server_conf->m_client_timeout;
        LOG(DEBUG) << "max_count:" << m_server_conf->m_max_count;
        LOG(DEBUG) << "script:" << m_server_conf->m_script;
        LOG(DEBUG) << "http_port:" << m_server_conf->m_http_port;
        LOG(DEBUG) << "</server>";
        */
        return true;
    } catch (std::exception& e) {
        LOG(ERROR) << "loadServer exception:" << e.what();
        return false;
    }
}

bool Config::loadDataBase(const tinyxml2::XMLNode& database)
{
    m_database_conf = std::make_shared<DataBaseConf>();
    try {
        for (auto* node = database.FirstChildElement("pool"); node; node = node->NextSiblingElement("pool")) {
            auto pool = std::make_shared<DBPool>();
            pool->m_id = node->FindAttribute("id")->Value();
            pool->m_init_size = static_cast<size_t>(node->FindAttribute("init_size")->IntValue());
            pool->m_max_size = static_cast<size_t>(node->FindAttribute("max_size")->IntValue());

            pool->m_name = node->FirstChildElement("name")->GetText();
            pool->m_ip = node->FirstChildElement("ip")->GetText();
            pool->m_user = node->FirstChildElement("user")->GetText();
            pool->m_password = node->FirstChildElement("password")->GetText();
            pool->m_port = boost::convert<uint16_t>(node->FirstChildElement("port")->GetText()).value();
            pool->m_charset = node->FirstChildElement("charset")->GetText();

            m_database_conf->m_pools[pool->m_id] = pool;
            /*
            LOG(DEBUG) << pool->m_id;
            LOG(DEBUG) << pool->m_init_size;
            LOG(DEBUG) << pool->m_max_size;

            LOG(DEBUG) << pool->m_name;
            LOG(DEBUG) << pool->m_ip;
            LOG(DEBUG) << pool->m_user;
            LOG(DEBUG) << pool->m_password;
            LOG(DEBUG) << pool->m_port;
            LOG(DEBUG) << pool->m_charset;
            */
        }
        return true;
    } catch (const std::exception& e) {
        LOG(ERROR) << "loadDataBase exception:" << e.what();
        return false;
    }
}

bool Config::loadRedis(const tinyxml2::XMLNode& redis)
{
    m_redis_conf = std::make_shared<RedisConf>();
    try {
        for (auto* node = redis.FirstChildElement(); node; node = node->NextSiblingElement()) {
            auto r = std::make_shared<RedisNode>();
            r->m_id = node->FindAttribute("id")->Value();
            r->m_ip = node->FirstChildElement("ip")->GetText();
            r->m_port = boost::convert<uint16_t>(node->FirstChildElement("port")->GetText()).value();
            m_redis_conf->m_redis[r->m_id] = r;
        }
        return true;
    } catch (const std::exception& e) {
        LOG(ERROR) << "loadRedis exception:" << e.what();
        return false;
    }
}

/*
bool Config::loadRedisScript(const boost::property_tree::ptree& pt)
{
    auto redis_child = pt.get_child_optional("redis");
    if (!redis_child)
        return true;
    for (auto& redis_it : *redis_child) {
        if (redis_it.first == "node") {
            const auto& node_child = redis_it.second;
            auto id = node_child.get<std::string>(XMLATTR_DOT + "id");

            if (!m_redis_conf) {
                LOG_FMT(ERROR, "no redis node %s", id.c_str());
                return false;
            }
            auto redis_node = m_redis_conf->findNode(id);
            if (!redis_node) {
                LOG_FMT(ERROR, "no redis node %s", id.c_str());
                return false;
            }
            for (auto& file_it : node_child) {
                if (file_it.first == "file") {
                    auto file_id = file_it.second.get<std::string>(XMLATTR_DOT + "id");
                    auto file_value = file_it.second.get_value<std::string>();
                    redis_node->m_files[file_id] = file_value;
                }
            }
        }
    }
    return true;
}
*/

}
