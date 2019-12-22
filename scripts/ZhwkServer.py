
'''
    简单的http服务端，用来处理智慧网咖更新器发送的http请求
'''

from urllib.request import Request
import json
from http.server import  HTTPServer, SimpleHTTPRequestHandler, ThreadingHTTPServer, HTTPStatus
from http.client import HTTPConnection
import time
import datetime
import threading
import logging.config
import os
import sys
from logging.handlers import TimedRotatingFileHandler

LOG_MODULE_NAME = "zhwklog"
HTTP_HOST = "127.0.0.1"
HTTP_PORT = 8080

VERSION = "0.1"

def GetLog():
    return logging.getLogger(LOG_MODULE_NAME)


def InitLogToFile():
    LOGFILE_DIR = os.path.join("./zhwklog/")
    if not os.path.exists(LOGFILE_DIR):
        os.mkdir(LOGFILE_DIR)
    LOGFILE = os.path.join(LOGFILE_DIR, "zhwkserver.log")

    STANDARD_FORMAT = '[%(asctime)s] [%(threadName)s:%(thread)d] [%(filename)s:%(lineno)d] ' \
                      '[%(levelname)s] %(message)s'
    fmt = logging.Formatter(STANDARD_FORMAT)
    hdl = TimedRotatingFileHandler(filename=LOGFILE, when='H')
    hdl.setFormatter(fmt)
    logger = logging.getLogger(LOG_MODULE_NAME)
    logger.addHandler(hdl)


def InitLogToConsole():
    STANDARD_FORMAT = '[%(asctime)s] [%(threadName)s:%(thread)d] [%(filename)s:%(lineno)d] ' \
                      '[%(levelname)s] %(message)s'
    fmt = logging.Formatter(STANDARD_FORMAT)
    hdl = logging.StreamHandler(stream=sys.stdout)
    hdl.setFormatter(fmt)
    logger = logging.getLogger(LOG_MODULE_NAME)
    logger.addHandler(hdl)


class ErrorCode:
    ContentError = (1000, "content error")
    ServerError = (2000, "server error")


class HttpHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path=='/zhwk/version':
            t = datetime.datetime.now()
            current_tm = t.strftime("%Y-%m-%d %H:%M:%S")
            self.ResponseJson(data={'localtime':current_tm, 'version:': VERSION})
        else:
            self.send_error(code=HTTPStatus.FORBIDDEN, message="forbidden")

    def do_POST(self):
        try:
            content_length =int(self.headers['content-length'])
            post_data = str(self.rfile.read(content_length).decode('utf-8'))
            self.DispatchPost(path=self.path, content_length=content_length, content=post_data)
        except (BaseException) as e:
            self.SendError(ErrorCode.ContentError)
            GetLog().warning("exception: {}".format(e))

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
        if path == '/zhwk/echo':
            self.HandlerUpdateEcho(content=content)
        elif path =='/zhwk/report':
            self.HandlerUpdateReport(content=content)
        else:
            self.send_error(code=HTTPStatus.FORBIDDEN, message="forbidden")
            GetLog().warning("unknown handler: {} {}".format(path, content))

#### echo
    def HandlerUpdateEcho(self, content=""):
        self.ResponseJson(data=content)
        GetLog().info("HandlerUpdateEcho: {}".format(content))

#### 上报信息
    def HandlerUpdateReport(self, content=""):
        GetLog().info("HandlerUpdateReport: {}".format(content))
        self.SendSuccess(data="")


def ReadConfig():
    fname = "./zhwkserver.json"
    try:
        with open(fname, "rb") as f:
            data = json.load(f)
            return data
    except (BaseException) as e:
        GetLog().warning('read config "{}" failed.'.format(fname))
        return None


def Start(*args, **kwargs):
    conf = kwargs["conf"]
    #url = "http://" + conf["host"] + ":" + conf["port"] + "/zhwk/datetime"
    #print(url)
    while True:
        time.sleep(30)
        conn = HTTPConnection(conf["host"], conf["port"], timeout=5)
        conn.request("GET", "/zhwk/version")
        resp = conn.getresponse()
        data = resp.read()
        GetLog().info("GET /zhwk/version: {}".format(data))


def StartTestThread(conf):
    temp = threading.Thread(name="TestThread", target=Start, kwargs={"conf": conf})
    temp.start()


def Main():
    InitLogToFile()
    #InitLogToConsole()
    GetLog().setLevel(logging.INFO)
    js = ReadConfig()
    if js is None:
        return

    if int(js["test"]) != 0:
        GetLog().info("start test thread")
        StartTestThread(js)
    else:
        GetLog().info("don't launch test thread")

    try:
        server_address = (js["host"], int(js["port"]))
        GetLog().info("http server start on port: {} version: {}".format(server_address, VERSION))
        #tserver = HTTPServer ThreadingHTTPServer(server_address, HttpHandler)
        tserver = HTTPServer(server_address, HttpHandler)
        tserver.serve_forever()
    except (BaseException) as e:
        pass

    GetLog().info("http server shutdown!")


if __name__ == '__main__':
    Main()

