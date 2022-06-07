#include "../include/server.h"
#include "../../utils/utils.h"


NFtp::TServer::TServer() {

    if (!InitAndRun()) {
        throw std::runtime_error("Couldn't init server");
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

        if (!fork()) { // this is the child process
            close(SockFd); // child doesn't need the listener
            ServeRequest(connectorFd);
            close(connectorFd);
            exit(0);
        }
        close(connectorFd);  // parent doesn't need this
    }
}

void NFtp::TServer::SendFile(const std::string& filename, int connectorFd) {
    FILE* fptr = fopen(filename.c_str(), "r");
    char buf[MaxDataSize];
    if (!fptr) {
        printf("Something went wrong with read()! %s\n", strerror(errno));
        exit(1);
    }

    while (!feof(fptr)) {
        int bytesRead = fread(buf, 1, sizeof(buf), fptr); 
        printf("read data: %d\n", bytesRead);
        if (send(connectorFd, buf, bytesRead, 0) == -1)
            perror("send");
    }

    fclose(fptr);
}

void NFtp::TServer::ServeRequest(int conFd) {
    NUtils::TFtpMessage msg;
    std::vector<char> buf(MaxDataSize);

    if (recv(conFd, &buf[0], buf.size(), 0) < 0) {
        perror("Couldn't recv TFtpMsg");
        return;
    }
    printf("Received: %s\n", buf.data());
    SendFile("/home/dinkambarov/proj/ftp/server/file.txt", conFd);
}