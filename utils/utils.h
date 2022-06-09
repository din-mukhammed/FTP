#pragma once
#include <bits/stdc++.h>
#include <netinet/in.h>


namespace NUtils {

enum ECommand {
    List = 0,
    Download = 1,
    Upload = 2,
    Login = 3
};

void* GetInAddr(sockaddr* sa);

void SaveFile(int fd, const std::string& newFileName, int maxDataSize);

void SendFile(int fd, const std::string& filename, int maxDataSize);

} // NUtils