#include "HttpServer.h"

#include <sstream>
#include <json/json.h>
#include <boost/format.hpp>

#include "GameTool.h"
#include "ServerApp.h"
#include "AppLog.h"
#include "Config.h"
#include "GlobalInstance.h"
#include "GameDefine.h"
#include "TCharge.h"
#include "IpTableConf.h"
#include "UtilsService.h"

#include "WechatNotify.h"
#include "WechatOrder.h"
#include "AlipayNotify.h"
#include "AlipayOrder.h"

namespace http {

HttpServer::HttpServer()
    : m_is_running(false)
    , m_actions()
    , m_thread()
    , m_http_server(nullptr)
{
    m_actions = 
    {
        {"^/serverinfo$",          "GET",    std::bind(&HttpServer::serverInfo, std::placeholders::_1, std::placeholders::_2)},
        {"^/charge/wechatOrder$",  "POST",   std::bind(&HttpServer::wechatOrder, std::placeholders::_1, std::placeholders::_2)},
        {"^/charge/wechatNotify$", "POST",   std::bind(&HttpServer::wechatNotify, std::placeholders::_1, std::placeholders::_2)},
        {"^/charge/alipayOrder$",  "POST",   std::bind(&HttpServer::alipayOrder, std::placeholders::_1, std::placeholders::_2)},
        {"^/charge/alipayNotify$", "POST",   std::bind(&HttpServer::alipayNotify, std::placeholders::_1, std::placeholders::_2)},
        {"^/charge/webOrder$",     "POST",   std::bind(&HttpServer::webOrder, std::placeholders::_1, std::placeholders::_2)},
    };
}

HttpServer::~HttpServer()
{
    stop();
    if (m_thread.joinable())
        m_thread.join();
}

bool HttpServer::init()
{
    if (m_is_running)
        return false;
    appLog(LOG_TIP, "init start");

    const int PORT = GlobalInstance::getConfig().getServerConf()->m_port;
    try{
        m_http_server.reset(new SWHttpServer((unsigned short)PORT, 1));
        m_http_server->default_resource["POST"] = std::bind(&HttpServer::httpDefaultPost, std::placeholders::_1, std::placeholders::_2);
        m_http_server->default_resource["GET"] = std::bind(&HttpServer::httpDefaultGet, std::placeholders::_1, std::placeholders::_2);
        for (auto& act : m_actions) {
            m_http_server->resource[act.m_path][act.m_method] = act.m_callback;
        }
        m_thread = std::thread([this]() { m_http_server->start(); });
        appLog(LOG_TIP, "port %d end", PORT);
    } catch (const std::exception& e) {
        appLog(LOG_ERROR, "port %d failed %s", PORT, e.what());
        return false;
    }
    return true;
}

void HttpServer::stop()
{
    try {
        if (m_http_server)
            m_http_server->stop();
    } catch (const std::exception& e) {
        appLog(LOG_ERROR, "failed %s", e.what());
    }
}

void HttpServer::waitStop()
{
    if (m_thread.joinable())
        m_thread.join();
}

void HttpServer::httpDefaultPost(ResponsePtr response, RequestPtr request)
{
    (void)request;
    std::string content = 
        "<HTML><TITLE>Not Found</TITLE>"
        "<BODY><P>The server could not fulfill<br>"
        "your request because the resource specified is unavailable or nonexistent."
        "</BODY></HTML>";
    *response << "HTTP/1.0 404 NOT FOUND\r\n"
        << "Content-Length: " << content.length() << "\r\n"
        << "\r\n" << content;
}

void HttpServer::httpDefaultGet(ResponsePtr response, RequestPtr request)
{
    httpDefaultPost(response, request);
}

void HttpServer::send200(ResponsePtr response, const std::string& s)
{
    *response << "HTTP/1.0 200 OK\r\n"
        << "Content-Length: " << s.size() << "\r\n"
        << "\r\n" << s;
}

void HttpServer::send200(ResponsePtr response, std::ostream& stem)
{
    *response << "HTTP/1.0 200 OK\r\n"
        << "Content-Length: " << stem.tellp() << "\r\n"
        << "\r\n" << stem.rdbuf();
}

void HttpServer::info(ResponsePtr response, RequestPtr request)
{
    std::stringstream content_stream;
    content_stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
    content_stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";
    for (auto& header : request->header) {
        content_stream << header.first << ": " << header.second << "<br>";
    }
    content_stream.seekp(0, std::ios::end);
    send200(response, content_stream);
}

void HttpServer::serverInfo(ResponsePtr response, RequestPtr request)
{
    (void)request;
    std::ostringstream ostm;
    try {
        ostm << boost::format("version:%0%\n") % gamedef::GAME_VERSION;
        ostm << boost::format("start time:%0%\n") % GlobalInstance::getStartTime();
    } catch (const std::exception& e) {
        (void)e;
    }
    send200(response, ostm.str());
}

void HttpServer::wechatOrder(ResponsePtr response, RequestPtr request)
{
    auto content = request->content.string();
    auto client_order = parseOrder(content, request);
    if (!client_order) {
        appLog(LOG_ERROR, "parse order %s", content.c_str());
        wechat::WechatOrder::sendRespError(response);
        return;
    }

    if (!GlobalInstance::getTCharge().validRMB(client_order->amount)) {
        appLog(LOG_ERROR, "rmb valid order %s", content.c_str());
        wechat::WechatOrder::sendRespError(response);
        return;
    }

    auto client_str = client_order->toString();
    wechat::WechatOrder order{std::move(response), std::move(request), std::move(client_order)};
    if (order.execute()) {
        appLog(LOG_DEBUG, "wechat order success [%s]", client_str.c_str());
    } else {
        appLog(LOG_ERROR, "wechat order fail [%s]", client_str.c_str());
    }
}

void HttpServer::wechatNotify(ResponsePtr response, RequestPtr request)
{
    auto content = request->content.string();
    wechat::WechatNotify notify{std::move(response), std::move(request), content};
    if (notify.execute()) {
        //appLog(LOG_DEBUG, "wechat notify success:%s", content.c_str());
    } else {
        appLog(LOG_ERROR, "wechat notify fail:%s", content.c_str());
    }
}

void HttpServer::alipayOrder(ResponsePtr response, RequestPtr request)
{
    auto content = request->content.string();
    auto client_order = parseOrder(content, request);
    if (!client_order) {
        appLog(LOG_ERROR, "parse order %s", content.c_str());
        alipay::AlipayOrder::sendRespError(response);
        return;
    }

    if (!GlobalInstance::getTCharge().validRMB(client_order->amount)) {
        appLog(LOG_ERROR, "rmb valid order %s", content.c_str());
        alipay::AlipayOrder::sendRespError(response);
        return;
    }

    auto client_str = client_order->toString();
    alipay::AlipayOrder order{std::move(response), std::move(request), std::move(client_order)};
    if (order.execute()) {
        appLog(LOG_DEBUG, "alipay order success [%s]", client_str.c_str());
    } else {
        appLog(LOG_ERROR, "alipay order fail [%s]", client_str.c_str());
    }
}

void HttpServer::alipayNotify(ResponsePtr response, RequestPtr request)
{
    auto content = request->content.string();
    alipay::AlipayNotify notify{std::move(response), std::move(request), content};
    if (notify.execute()) {
        //appLog(LOG_DEBUG, "alipay notify success:%s", content.c_str());
    } else {
        appLog(LOG_ERROR, "alipay notify fail:%s", content.c_str());
    }
}

void HttpServer::webOrder(ResponsePtr response, RequestPtr request)
{
    uint64_t userid = 0;
    int32_t rmb = 0;
    auto content = request->content.string();
    try {
        Json::Value json_val{};
        Json::Reader reader{};
        if (!reader.parse(content, json_val)) {
            send200(response, "parse json error");
            return;
        }

        userid = json_val["userid"].asUInt64();
        rmb = json_val["rmb"].asInt();
    } catch (const Json::Exception& e) {
        send200(response, "userid or rmb is null");
        return;
    }

    if (userid == 0 || rmb <= 0) {
        send200(response, "userid or rmb value error");
        return;
    }

    if (GlobalInstance::getUtilsService().webOrder(userid, rmb)) {
        send200(response, "success");
    } else {
        send200(response, "charge error");
    }
}

ClientOrderPtr HttpServer::parseOrder(const std::string& content, const RequestPtr& request)
{
    std::string client_ip{};
    //获取客户端真实ip
    auto it_range = request->header.equal_range("X-real-ip");
    auto it_begin = it_range.first;
    auto it_end = it_range.second;
    for (; it_begin != it_end; ++it_begin) {
        std::string ip = it_begin->second;
        if (!ip.empty())
            client_ip = std::move(ip);
    }

    try {
        Json::Value json_val;
        Json::Reader reader{};
        if (!reader.parse(content, json_val)) {
            appLog(LOG_ERROR, "content %s", content.c_str());
            return nullptr;
        }
        auto order = std::make_shared<ClientOrder>();
        order->amount    = json_val["amount"].asInt();
        order->user_id   = json_val["user_id"].asUInt64();
        order->charge_id = json_val["charge_id"].asInt();
        order->imsi     = json_val["imsi"].asString();
        order->imei     = json_val["imei"].asString();
        order->channel  = json_val["channel"].asString();
        order->version  = json_val["version"].asString();
        order->model    = json_val["model"].asString();
        order->os       = json_val["os"].asInt();
        order->game_id  = json_val["game_id"].asInt();

        if (order->amount <= 0 || order->user_id == 0) {
            appLog(LOG_ERROR, "content %s", content.c_str());
            return nullptr;
        }

        order->ip       = std::move(client_ip);
        order->telecom  = gametool::getTelecom(order->imsi);
        order->addr     = GlobalInstance::getIpTableConf().findLocation(order->ip);
        return order;
    } catch (const std::string& e) {
        appLog(LOG_ERROR, "content %s", content.c_str());
        return nullptr;
    }
}

}
