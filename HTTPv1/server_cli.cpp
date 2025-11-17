#include <iostream>
#include <iomanip>
#include "server_cli.h"
#include "HttpServer.h"
using namespace std;

server_cli::server_cli():CmdLineInterface("Server>"){
    httpServer = new HttpServer(8888);
    initCmd();
}

server_cli::~server_cli(){
    delete httpServer;
}

void server_cli::initCmd(){
    addCmd("start",CLI_CAST(&server_cli::doStart));
    addCmd("stop",CLI_CAST(&server_cli::doStop));
    addCmd("help",CLI_CAST(&server_cli::doHelp));
}

void server_cli::doStart(string cmd_argv[], int cmd_argc){
    if (cmd_argc != 1) {
        cout<<"Invalid command. Using help "<<endl;
        return;
    }
    if (httpServer->isRunning()){
        cout<<"Server is running"<<endl;
    } else {
        if (httpServer->start()){
            cout<<"Server started running"<<endl;
        } else cout<<"Server failed to start"<<endl;
    }
}

void server_cli::doStop(string cmd_argv[], int cmd_argc){
    if (cmd_argc != 1) {
        cout<<"Invalid command. Using help "<<endl;
        return;
    }
    httpServer->stop();
    cout <<"Server stopped"<<endl;
}

void server_cli::doHelp(string cmd_argv[], int cmd_argc){
    cout << left << setw(10) << "start" << ": starting server" << endl;
    cout << left << setw(10) << "stop"  << ": stop server" << endl;
    cout << left << setw(10) << "help"  << ": print information of command" << endl;
}
