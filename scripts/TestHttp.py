import http.server
import socketserver


def Main():
    PORT = 6079
    Handler = http.server.SimpleHTTPRequestHandler
    with socketserver.TCPServer(("192.168.97.15", PORT), Handler) as httpd:
        print("serving at port", PORT)
        httpd.serve_forever()


if __name__ == '__main__':
    Main()