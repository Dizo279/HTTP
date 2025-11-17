#include "HttpHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

static string getContentType(const string& path){
	string lower = path;
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	if(lower.find(".jpg")!=string::npos || lower.find(".jpeg")!=string::npos) return "image/jpeg";
	if(lower.find(".png")!=string::npos) return "image/png";
	if(lower.find(".gif")!=string::npos) return "image/gif";
	return "application/octet-stream";
}

string HttpHandler::handleRequest(const string& request){
	// request is expected like: GET /file.jpg HTTP/1.1
	istringstream iss(request);
	string method, uri, proto;
	iss >> method >> uri >> proto;
	if(method != "GET"){
		string body = "<html><body><h1>400 Bad Request</h1></body></html>";
		ostringstream resp;
		resp << "HTTP/1.1 400 Bad Request\r\n";
		resp << "Content-Length: "<<body.size()<<"\r\n";
		resp << "Connection: close\r\n";
		resp << "Content-Type: text/html\r\n\r\n";
		resp << body;
		return resp.str();
	}

	// strip leading '/'
	string filename = uri;
	if(!filename.empty() && filename[0]=='/') filename.erase(0,1);
	if(filename.empty()) filename = "index.html";

	string baseDir = "images"; // images folder relative to working dir
	// if client already requested images/..., don't prefix again
	string fullpath;
	if(filename.rfind(baseDir + "/", 0) == 0) {
		// filename already begins with "images/"
		fullpath = filename;
	} else {
		fullpath = baseDir + "/" + filename;
	}

	// Basic security: do not allow path traversal
	if(fullpath.find("..") != string::npos) {
		string body = "<html><body><h1>403 Forbidden</h1></body></html>";
		ostringstream resp;
		resp << "HTTP/1.1 403 Forbidden\r\n";
		resp << "Content-Length: "<<body.size()<<"\r\n";
		resp << "Connection: close\r\n";
		resp << "Content-Type: text/html\r\n\r\n";
		resp << body;
		return resp.str();
	}

	ifstream file(fullpath, ios::binary);
	if(!file.is_open()){
		string body = "<html><body><h1>404 Not Found</h1></body></html>";
		ostringstream resp;
		resp << "HTTP/1.1 404 Not Found\r\n";
		resp << "Content-Length: "<<body.size()<<"\r\n";
		resp << "Connection: close\r\n";
		resp << "Content-Type: text/html\r\n\r\n";
		resp << body;
		return resp.str();
	}

	// read file contents
	vector<char> buf;
	file.seekg(0, ios::end);
	streamsize size = file.tellg();
	file.seekg(0, ios::beg);
	if(size>0){
		buf.resize((size_t)size);
		file.read(&buf[0], size);
	}

	string contentType = getContentType(fullpath);
	ostringstream headers;
	headers << "HTTP/1.1 200 OK\r\n";
	headers << "Content-Length: " << buf.size() << "\r\n";
	headers << "Connection: close\r\n";
	headers << "Content-Type: " << contentType << "\r\n\r\n";

	string response = headers.str();
	if(!buf.empty()){
		response.append(&buf[0], buf.size());
	}
	return response;
}


