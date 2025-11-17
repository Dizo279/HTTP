#include "cli.h"
#include "HttpServer.h"

class server_cli : public CmdLineInterface{
private:
    HttpServer* httpServer;
public:
    server_cli();
    ~server_cli();
protected:
    virtual void initCmd();

private:
    void doStart(string cmd_argv[], int cmd_argc);
    void doStop(string cmd_argv[], int cmd_argc);
    void doHelp(string cmd_argv[], int cmd_argc);

};

