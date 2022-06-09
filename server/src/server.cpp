#include "../include/server.h"
#include <dirent.h>


NFtp::TServer::TServer() {
    if (!InitAndRun()) {
        printf("Couldn't init server!\n");
    }
}

int NFtp::TServer::InitAndRun() {
    struct addrinfo hints, *servinfo, *p;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((SockFd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(SockFd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(SockFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(SockFd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(SockFd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    ServeForever();

    return 0;
}

void NFtp::TServer::ServeForever() {
    struct sockaddr_storage connectorAddr;
    socklen_t sinSize;
    char s[INET6_ADDRSTRLEN];

    while (true) {
        sinSize = sizeof connectorAddr;
        int connectorFd = accept(SockFd, (sockaddr *)&connectorAddr, &sinSize);
        if (connectorFd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(connectorAddr.ss_family,
            NUtils::GetInAddr((sockaddr *)&connectorAddr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { 
            close(SockFd); 
            ServeRequest(connectorFd);
            close(connectorFd);
            exit(0);
        }
        close(connectorFd); 
    }
}

void NFtp::TServer::ServeRequest(int conFd) {
    char buf[MaxDataSize];
    int numbytes = 0;

    if (numbytes = recv(conFd, &buf[0], sizeof(buf), 0); numbytes < 0) {
        perror("Couldn't recv TFtpMsg");
        return;
    }
    buf[numbytes] = '\0';
    auto clientMsg = NUtils::TFtpMessage::ToMsg(buf);
    printf("Received Message: size = %zu\n", clientMsg.ToStr().size());

    int commandCode = std::stoi(clientMsg.GetParam("command"));
    switch (commandCode) {
    case NUtils::ECommand::Download:
        SendFile(clientMsg, conFd);
        break;
    case NUtils::ECommand::List:
        ListFiles(clientMsg, conFd);
        break;
    case NUtils::ECommand::Upload:
        SaveFile(clientMsg, conFd);
        break;
    default:
        printf("Invalid command: %d", commandCode);
    }
}

void NFtp::TServer::SendFile(const NUtils::TFtpMessage& clientMsg, int conFd) {
    auto userFiles = GetUserFiles("../" + clientMsg.GetParam("userId"));
    auto it = find(userFiles.begin(), userFiles.end(), clientMsg.GetParam("filename"));
    assert(it != userFiles.end());
    NUtils::SendFile(
        conFd, 
        DefaultPath + clientMsg.GetParam("userId") + "/" + clientMsg.GetParam("filename"), 
        MaxDataSize);
}

void NFtp::TServer::ListFiles(const NUtils::TFtpMessage& clientMsg, int conFd) {
    auto userFiles = GetUserFiles("../" + clientMsg.GetParam("userId"));
    std::string listFiles;
    for (size_t i = 0; i < userFiles.size(); ++i) {
        listFiles += (i > 0 ? "," : "") + userFiles[i];
    }
    std::unordered_map<std::string, std::string> resp = {
        {"listFiles", listFiles}
    };
    assert(NUtils::TFtpMessage(resp).Send(conFd));
}

std::vector<std::string> NFtp::TServer::GetUserFiles(const std::string& userId) {
    std::vector<std::string> files;
    DIR* curDir = opendir(userId.c_str());
    if (!curDir) {
        perror(userId.c_str());
        exit(1);
    }
    while (auto dir = readdir(curDir)) {
        if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            printf(">> %s: %s\n", userId.c_str(), dir->d_name);
            files.push_back(dir->d_name);
        }
    }
    closedir(curDir);
    return files;
}

void NFtp::TServer::SaveFile(const NUtils::TFtpMessage& clientMsg, int conFd) {
    std::unordered_map<std::string, std::string> params = {
        {"status", "OK"}
    };
    assert(NUtils::TFtpMessage(params).Send(conFd));
    auto path = DefaultPath + clientMsg.GetParam("userId") + "/" + clientMsg.GetParam("filename");
    NUtils::SaveFile(conFd, path, MaxDataSize);
    assert(NUtils::TFtpMessage(params).Send(conFd));
}