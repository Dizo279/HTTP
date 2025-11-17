#include "tcpclient.h"
#include <string>
using namespace std;

class ResourceDownload : public TcpClient {
public:
    ResourceDownload();
    ~ResourceDownload() {}
    void loadFileList(const string& fileName, const string& serverIp, const string& port);

private:
    bool downloadFile(const string& fileName, const string& serverIp, const string& port);
};