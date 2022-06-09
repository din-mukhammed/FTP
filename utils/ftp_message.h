#pragma once
#include "utils.h"

namespace NUtils {

class TFtpMessage {
public:
    TFtpMessage() = default;

    TFtpMessage(const std::unordered_map<std::string, std::string>& params);

    std::string ToStr() const;

    static TFtpMessage ToMsg(const std::string& msgStr);

    std::string GetParam(const std::string& key) const;

    bool Send(int sockFd) const;
    static bool RecvMsg(int sockFd, int maxDataSize, TFtpMessage* msg);

    static std::pair<std::string, std::string> ParsePair(const std::string& pairStr);
    static std::vector<std::string> ParseList(const std::string& listStr);
private:
    std::unordered_map<std::string, std::string> Params;
    
    const static char PairDelim = '=';
    const static char ArgDelim = '#';
    const static char ListDelim = ',';


};
} // NUtils