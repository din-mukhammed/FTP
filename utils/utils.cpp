#include "utils.h"

using namespace NUtils;
using std::string;

void* NUtils::GetInAddr(sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in*)sa)->sin_addr);
    }

    return &(((sockaddr_in6*)sa)->sin6_addr);
}

void NUtils::SaveFile(int fd, const std::string& newFileName, int maxDataSize) {
    FILE* fptr = fopen(newFileName.c_str(), "w");

    if (!fptr) {
        printf("Something went wrong with fopen()! newFileName: %s, error: %s\n", newFileName.c_str(), strerror(errno));
        exit(1);
    }

    int numbytes = 0;
    int sum = 0;
    std::vector<char> buf(maxDataSize);
    while ((numbytes = recv(fd, &buf[0], buf.size(), 0)) > 0) {
        sum += numbytes;
        printf("received: '%d bytes\n", numbytes);
        assert(fwrite(&buf[0], sizeof(char), numbytes, fptr) == numbytes);
    }
    printf("Total bytes received: '%d'\n", sum);

    fclose(fptr);
}

void NUtils::SendFile(const std::string& filename, int connectorFd, int maxDataSize) {
    FILE* fptr = fopen(filename.c_str(), "r");
    std::vector<char> buf(maxDataSize);
    if (!fptr) {
        printf("Something went wrong with read()! filename: %s, error: %s\n", filename.c_str(), strerror(errno));
        exit(1);
    }

    int sum = 0;
    int numbytes = 0;

    while (!feof(fptr)) {
        numbytes = fread(&buf[0], 1, buf.size(), fptr); 
        sum += numbytes;
        printf("read data: %d\n", numbytes);
        if (send(connectorFd, &buf[0], numbytes, 0) == -1)
            perror("send");
    }
    printf("Total read data: %d\n", sum);
    fclose(fptr);
}