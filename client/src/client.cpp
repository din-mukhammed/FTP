#include "../include/client.h"
#include "../../utils/utils.h"


TClient::TClient(const std::string& hostname) {
    if (int code = Init(hostname); code) {
        throw std::runtime_error("Couldn't init client: " + std::to_string(code));
    }
}

TClient::~TClient() {
    close(SockFd);
}

int TClient::Init(const std::string& hostname) {
    int numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname.c_str(), PORT, &hints, &servinfo)) != 0) {
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

    freeaddrinfo(servinfo); // all done with this structure
    DownloadFile("");

    return 0;
}

void TClient::DownloadFile(const std::string& newFileName) {
    std::unordered_map<std::string, std::string> params = {
        {"filename", "/home/dinkambarov/proj/ftp/server/file.txt"}
    };
    NUtils::TFtpMessage msg("Din", "List", std::move(params));
    auto msgStr = msg.ToStr();
    int bytesRead = msgStr.size();
    printf("Sending TFtpMessage: %s\n", msgStr.c_str());
    if (send(SockFd, msgStr.c_str(), bytesRead, 0) < 0) {
        perror("send TFtpMessage");
        return;
    }

    std::vector<char> buf(MaxDataSize);
    int numbytes = 0;
    int sum = 0;
    while ((numbytes = recv(SockFd, &buf[0], MaxDataSize, 0)) > 0) {
        sum += numbytes;
        printf("client: received '%d, content:\n", numbytes);
        for (char ch : buf)
            printf("%c", ch);
    }
    printf("TOTAL client: received '%d'\n", sum);
}
