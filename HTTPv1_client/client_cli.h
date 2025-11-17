#include "cli.h"
#include "tcpsocket.h"
class client_cli : public CmdLineInterface{
private:
public:
    client_cli();
    ~client_cli();
protected:
    virtual void initCmd();

private:
    void doLoad(string cmd_argv[], int cmd_argc);
    void doHelp(string cmd_argv[], int cmd_argc);
};

