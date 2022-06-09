#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <bits/stdc++.h>

class TClient {
public:
    TClient(const std::string& userId);

    ~TClient();
    
    void DownloadFile(const std::string& newFileName);
    void UploadFile(const std::string& fileName);
    std::vector<std::string> ListFiles();
private:
    int Init();

    const std::string UserId;
    const char* PORT = "3490";
    const int MaxDataSize = 1024;
    int SockFd = 0;
};