#ifndef _HTTP_HTTPSERVER_H_
#define _HTTP_HTTPSERVER_H_

#include <thread>
#include <memory>
#include <map>
#include<vector>
#include <atomic>

#include <unordered_map>

#include "SimpleWeb/server_http.hpp"
#include "HttpDefine.h"
#include "OrderType.h"

namespace http {

struct Action
{
    std::string     m_path;
    std::string     m_method;
    std::function<void(ResponsePtr, RequestPtr)> m_callback;

    Action() = default;
    ~Action() = default;
    Action(const Action& rhs) = default;
    Action& operator=(const Action& rhs) = default;

    Action(Action&& rhs) = default;
    Action& operator=(Action&& rhs) = default;
};

class HttpServer
{
public:
    HttpServer();
    ~HttpServer();

    bool init();
    void stop();
    void waitStop();

    static void httpDefaultPost(ResponsePtr response, RequestPtr request);
    static void httpDefaultGet(ResponsePtr response, RequestPtr request);

    static void send200(ResponsePtr response, const std::string& s);
    static void send200(ResponsePtr response, std::ostream& stem);
private:
    static void serverInfo(ResponsePtr response, RequestPtr request);
    static void info(ResponsePtr response, RequestPtr request);

    //微信支付下单
    static void wechatOrder(ResponsePtr response, RequestPtr request);
    static void wechatNotify(ResponsePtr response, RequestPtr request);

    //支付宝下单
    static void alipayOrder(ResponsePtr response, RequestPtr request);
    static void alipayNotify(ResponsePtr response, RequestPtr request);

    //web充值
    static void webOrder(ResponsePtr response, RequestPtr request);

    static ClientOrderPtr parseOrder(const std::string& content, const RequestPtr& request);
private:
    std::atomic<bool>               m_is_running;
    std::vector<Action>             m_actions;
    std::thread                     m_thread;
    std::unique_ptr<SWHttpServer>   m_http_server;
};

}

#endif