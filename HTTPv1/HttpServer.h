#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "tcpserver.h"

class HttpServer : public TCPServer {
public:
    HttpServer(unsigned short port = 8888);
    ~HttpServer() {};
protected:
    void startNewSession (TcpSocket slave);
    int run (TcpSocket &slave);
};

#endif // HTTPSERVER_H
