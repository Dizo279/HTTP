#include "ResourceDownload.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

ResourceDownload::ResourceDownload() : TcpClient() {}

void ResourceDownload::loadFileList(const string& fileName, const string& serverIp, const string& port) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        // Trim whitespace robustly
        size_t endpos = line.find_last_not_of(" \t\r\n");
        if (endpos != string::npos) line.erase(endpos + 1);
        else line.clear();
        size_t startpos = line.find_first_not_of(" \t\r\n");
        if (startpos != string::npos) line.erase(0, startpos);
        else line.clear();
        if (!line.empty() && line[0] != '#') {
            cout << "Downloading file: " << line << " from " << serverIp << ":" << port << endl;
            bool success = downloadFile(line, serverIp, port);
            if (success) {
                cout << "Successfully downloaded: " << line << endl;
            } else {
                cerr << "Failed to download: " << line << endl;
            }
        }
    }
    file.close();
}

bool ResourceDownload::downloadFile(const string& fileName, const string& serverIp, const string& port) {
    if (!open(serverIp, port)) {
        cerr << "Failed to connect to server: " << serverIp << ":" << port << endl;
        return false;
    }

    // Prepare the HTTP GET request
    string request = "GET /" + fileName + " HTTP/1.1\r\n";
    request += "Host: " + serverIp + "\r\n";
    request += "Connection: close\r\n\r\n";

    // Send the request
    if (sendStringRequest(request) < 0) {
        cerr << "Failed to send request for file: " << fileName << endl;
        close();
        return false;
    }

    // Read response status line
    char line[1024];
    int n = recvGetLine(line, sizeof(line));
    if (n <= 0) {
        cerr << "Failed to receive response status for file: " << fileName << endl;
        close();
        return false;
    }
    string statusLine(line);
    // parse status code
    int statusCode = 0;
    {
        istringstream iss(statusLine);
        string http;
        iss >> http >> statusCode;
    }

    // read headers
    int contentLength = -1;
    while(true){
        int h = recvGetLine(line, sizeof(line));
        if(h<=0) break;
        if(line[0]==0) break; // blank line
        string hdr(line);
        // lowercase header name for simple match
        string hk = hdr;
        transform(hk.begin(), hk.end(), hk.begin(), ::tolower);
        if(hk.find("content-length:")!=string::npos){
            size_t p = hdr.find(':');
            if(p!=string::npos){
                string val = hdr.substr(p+1);
                // trim
                size_t s = val.find_first_not_of(" \t");
                if(s!=string::npos) val = val.substr(s);
                contentLength = stoi(val);
            }
        }
    }

    vector<char> body;
    if(contentLength>0){
        body.reserve(contentLength);
        int left = contentLength;
        const int BUF_SZ = 4096;
        char buf[BUF_SZ];
        while(left>0){
            int toRead = (left>BUF_SZ)?BUF_SZ:left;
            int r = recvDataBuffer(buf, toRead+1);
            if(r<=0) break;
            body.insert(body.end(), buf, buf+r);
            left -= r;
        }
    } else {
        // read until connection close
        const int BUF_SZ = 4096;
        char buf[BUF_SZ];
        int r = recvDataBuffer(buf, BUF_SZ);
        while(r>0){
            body.insert(body.end(), buf, buf+r);
            r = recvDataBuffer(buf, BUF_SZ);
        }
    }

    if(statusCode != 200){
        cerr << "Server returned status "<<statusCode<<" for file "<<fileName<<"\n";
        close();
        return false;
    }

    // Save body to file (use basename if client listed a path)
    string localName = fileName;
    size_t pos = localName.find_last_of("/\\");
    if(pos != string::npos) localName = localName.substr(pos+1);

    if(localName.empty()){
        cerr << "Invalid local filename for: " << fileName << endl;
        close();
        return false;
    }

    // Save using basename only
    ofstream outFile(localName, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error opening output file: " << fileName << endl;
        close();
        return false;
    }
    if(!body.empty()) outFile.write(&body[0], body.size());
    outFile.close();

    close();
    return true;
}
