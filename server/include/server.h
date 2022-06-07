#pragma once

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

/*

    Server:
        Init()
        AcceptRequest()
        SendFile(filename)
*/
namespace NFtp {

class TServer {
public:
    TServer();
private:
    int InitAndRun();

    void ServeForever();
    void SendFile(const std::string& filename, int connectorFd);
    void ServeRequest(int connectorFd);

    const char* PORT = "3490";
    const int BACKLOG = 20;

    int SockFd = 0;
    const int MaxDataSize = 100;
};

} // NFtp