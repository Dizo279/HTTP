#include <iostream>
#include "client_cli.h"
#include "ResourceDownload.h"
using namespace std;

client_cli::client_cli():CmdLineInterface("Client>"){
    initCmd();
}
client_cli::~client_cli()
{
}
void client_cli::initCmd(){
    addCmd("load",CLI_CAST(&client_cli::doLoad));
    addCmd("help",CLI_CAST(&client_cli::doHelp));
}

void client_cli::doLoad(string cmd_argv[], int cmd_argc){
    if (cmd_argc != 4) {
        cout<<"Invalid command. Using help "<<endl;
        return;
    }
    string Name_source_file = cmd_argv[1];
    string Server_ip = cmd_argv[2];
    string Port_str = cmd_argv[3];
    
    ResourceDownload downloader;
    downloader.loadFileList(Name_source_file, Server_ip, Port_str);
}


void client_cli::doHelp(string cmd_argv[], int cmd_argc){
    cout<<"load <list_file> <server_ip> <port>     : Read list_file and download resource fron server"<<endl;
    cout<<"help     : print information of command"<<endl;
}
