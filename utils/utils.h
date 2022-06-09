#pragma once
#include <bits/stdc++.h>
#include <netinet/in.h>


namespace NUtils {

const char ParamDelim = '=';
const char ArgDelim = '#';

enum ECommand {
    List = 0,
    Download = 1,
    Upload = 2,
    Login = 3
};

void* GetInAddr(sockaddr* sa);

std::pair<std::string, std::string> ParsePair(const std::string& str);

void SaveFile(int fd, const std::string& newFileName, int maxDataSize);

void SendFile(const std::string& filename, int connectorFd, int maxDataSize);

} // NUtils