#include "HttpServer.h"
#include "HttpHandler.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

HttpServer::HttpServer(unsigned short port):TCPServer(port){
}


void HttpServer::startNewSession (TcpSocket slave){
    bool connection = true;
    while(connection){
        int r = run(slave);
        if(r <= 0){
            connection = false;
        }
    }
    slave.close();
}

int HttpServer::run (TcpSocket &slave){
    char line[1024];
    try{
        int n = slave.recvLine(line, sizeof(line));
        if(n<=0) return n;
        // remove trailing CRLF
        if(n>=2) line[n-2]=0;
        string requestLine(line);

        // read and discard remaining headers until blank line
        while(true){
            int h = slave.recvLine(line, sizeof(line));
            if(h<=0) break;
            if(h==2) break; // CRLF only
        }

        HttpHandler handler;
        string response = handler.handleRequest(requestLine);

        // send response (may contain binary data)
        if(!response.empty()){
            slave.send(response.c_str(), (int)response.size());
        }
        return (int)response.size();
    } catch (SocketException& e){
        cerr<<"Socket error in HttpServer::run: "<<e.what()<<endl;
        return -1;
    }
}
