
'''
    简单的http服务端
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

HTTP_HOST = "127.0.0.1"
HTTP_PORT = 8080

class ErrorCode:
    ContentError = (1000, "content error")
    ServerError = (2000, "server error")


class HttpHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path=='/test':
            n = 0
            while n < 10:
                time.sleep(1)
                n = n + 1
                print(n)
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
            print("exception: {}".format(e))

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
            print("unknown handler: {} {}".format(path, content))

#### echo
    def HandlerUpdateEcho(self, content=""):
        self.ResponseJson(data=content)
        print("HandlerUpdateEcho: {}".format(content))

#### 上报信息
    def HandlerUpdateReport(self, content=""):
        print("HandlerUpdateReport: {}".format(content))
        self.SendSuccess(data="")


def ReadConfig():
    fname = "./zhwkserver.json"
    try:
        with open(fname, "rb") as f:
            data = json.load(f)
            return data
    except (BaseException) as e:
        print('read config "{}" failed.'.format(fname))
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
        print("GET /zhwk/version: {}".format(data))


def StartTestThread(conf):
    temp = threading.Thread(name="TestThread", target=Start, kwargs={"conf": conf})
    temp.start()


def Main():
    try:
        server_address = (HTTP_HOST, HTTP_PORT)
        print("http server start on port: {}".format(server_address))
        tserver = HTTPServer(server_address, HttpHandler)
        tserver.serve_forever()
    except (BaseException) as e:
        pass
    print("http server shutdown!")


if __name__ == '__main__':
    Main()

