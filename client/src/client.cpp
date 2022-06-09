#include "../include/client.h"
#include "../../utils/ftp_message.h"


TClient::TClient(const std::string& userId)
: UserId(userId)
{
    if (int code = Init(); code) {
        printf("Couldn't init client: %d", code);
    }
}

TClient::~TClient() {
    close(SockFd);
}

int TClient::Init() {
    int numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("din", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((SockFd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(SockFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(SockFd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, NUtils::GetInAddr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    return 0;
}

void TClient::DownloadFile(const std::string& newFileName) {
    std::unordered_map<std::string, std::string> params = {
        {"filename", "1.cpp"},
        {"userId", "Din"},
        {"command", "1"}
    };
    assert(NUtils::TFtpMessage(params).Send(SockFd));
    NUtils::SaveFile(SockFd, newFileName, MaxDataSize);
}

std::vector<std::string> TClient::ListFiles() {
    std::unordered_map<std::string, std::string> params = {
        {"userId", UserId},
        {"command", "0"}
    };
    assert(NUtils::TFtpMessage(params).Send(SockFd));
    NUtils::TFtpMessage resp;
    assert(NUtils::TFtpMessage::RecvMsg(SockFd, MaxDataSize, &resp));
    return NUtils::TFtpMessage::ParseList(resp.GetParam("listFiles"));
}