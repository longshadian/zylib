
'''
    简单的http服务端，用来处理智慧网咖更新器发送的http请求
    包含:
        更新器上报本地服务，数据库版本号，更新器自身版本号等
'''

import urllib
import json
from http.server import  HTTPServer, SimpleHTTPRequestHandler, ThreadingHTTPServer, HTTPStatus
import time
import datetime

import ZhwkLogger
import DBManager

g_dbmgr = DBManager.DBManager()


class ErrorCode:
    ContentError = (1000,"content error")
    ServerError = (2000, "server error")


class HttpHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path=='/zhwk/datetime':
            t = datetime.datetime.now()
            current_tm = t.strftime("%Y-%m-%d %H:%M:%S")
            self.ResponseJson(data={'localtime':current_tm})
        else:
            self.send_error(code=HTTPStatus.FORBIDDEN, message="forbidden")

    def do_POST(self):
        try:
            content_length =int(self.headers['content-length'])
            post_data = str(self.rfile.read(content_length).decode('utf-8'))
            self.DispatchPost(path=self.path, content_length=content_length, content=post_data)
        except (BaseException) as e:
            self.SendError(ErrorCode.ContentError)
            ZhwkLogger.GetLog().warning("exception: {}".format(e))

    def SendSuccess(self, data={}):
        self.ResponseJson(ecode=0, emsg="success", data=data)

    def SendError(self, ec):
        self.ResponseJson(ecode=ec[0], emsg=ec[1])

    def ResponseJson(self, ecode=0, emsg="success", data={}):
        js = {}
        js['code'] = ecode
        js['msg'] = emsg
        js['data'] = data
        js_content = json.dumps(js)

        self.send_response(HTTPStatus.OK)
        #self.send_header('content-type', 'text/plain;charset=UTF-8')
        self.send_header('content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(js_content.encode())

    def DispatchPost(self, path="", content_length=0, content=""):
        #print('headers', self.headers)
        #print('command', self.command)
        if path == '/zhwkupdate/echo':
            self.HandlerUpdateEcho(content=content)
        elif path =='/zhwkupdate/report':
            self.HandlerUpdateReport(content=content)
        elif path =='/zhwkupdate/can_update':
            self.HandlerUpdateCanUpdate(content=content)
        else:
            self.send_error(code=HTTPStatus.FORBIDDEN, message="forbidden")
            ZhwkLogger.GetLog().warning("unknown handler: {} {}".format(path, content))

#### echo
    def HandlerUpdateEcho(self, content=""):
        self.ResponseJson(data=content)

#### 上报信息
    def HandlerUpdateReport(self, content=""):
        ZhwkLogger.GetLog().info("HandlerUpdateReport req: {}".format(content))
        try:
            js = json.loads(content)
            g_dbmgr.UpdateTableNetbar(js)
            self.SendSuccess()
        except (BaseException) as e:
            self.SendError(ErrorCode.ServerError)
            ZhwkLogger.GetLog().warning("HandlerUpdateReport exception:{}".format(e))


#### 请求当前网咖能否更新
    def HandlerUpdateCanUpdate(self, content=""):
        ZhwkLogger.GetLog().info("update can_update: ", content)
        shopid = 0
        try:
            js = json.loads(content)
            if "shopid" in js:
                shopid = int(js["shopid"])
            can_update = g_dbmgr.SelectNetbar(shopid)
            resp_data = {'cat_update':can_update}
            self.SendSuccess(data=resp_data)
        #except (RuntimeError, TypeError) as e:
        except (BaseException) as e:
            self.SendError(ErrorCode.ServerError)
            ZhwkLogger.GetLog().warning("HandlerUpdateCanUpdate exception:{}".format(e))


def Init():
    ZhwkLogger.InitLog()

    global g_dbmgr
    g_dbmgr = DBManager.DBManager()
    db_name = "./db/cms.db"
    if not g_dbmgr.Init(db_name):
        ZhwkLogger.GetLog().warning("init db failed. name:{}".format(db_name))
        return False
    return True


def Main():
    if not Init():
        ZhwkLogger.GetLog().warning("init failed.")
        return

    port = 8080
    ZhwkLogger.GetLog().info("http server start on port:{}".format(port))
    server_address=('', port)
    #tserver = HTTPServer ThreadingHTTPServer(server_address, HttpHandler)
    tserver = HTTPServer(server_address, HttpHandler)
    tserver.serve_forever()

    ZhwkLogger.GetLog().info("http server shutdown!")


if __name__ == '__main__':
    Main()
