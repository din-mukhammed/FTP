#include "utils.h"

using namespace NUtils;
using std::string;

TMessage::TMessage(const string& userId)
    : UserId(userId) 
    {}

std::string TMessage::ToStr() const {
    return "userId=" + UserId;
}
bool TMessage::ValidateArgumentsNum(const std::string& msgStr, int expectedArgsNum) {
    const int delimNum = std::count(msgStr.begin(), msgStr.end(), ArgDelim);
    return delimNum >= expectedArgsNum;
}

std::pair<string, string> TMessage::ParsePair(const std::string& str) {
    std::pair<string, string> ans;
    int delimPos = str.find(ParamDelim);
    ans.first = str.substr(0, delimPos);
    ans.second = str.substr(delimPos + 1);
    return ans;
}

TMessage TMessage::ToMsg(const std::string& msgStr) {
    std::istringstream ss(msgStr);
    std::string token;

    if (!ValidateArgumentsNum(msgStr, 1)) {
        throw std::runtime_error("Couldn't parse: " + msgStr);
    }

    std::getline(ss, token, ArgDelim);
    auto [_, userId] = ParsePair(token);
    return TMessage(userId);
}

TFtpMessage::TFtpMessage(
    const std::string& userId,
    const std::string& command,
    std::unordered_map<std::string, std::string>&& params)
    : TMessage(userId)
    , Command(command)
    , Params(std::move(params))
{
}

std::string TFtpMessage::ToStr() const {
    std::stringstream ss;
    ss << "userId" << ParamDelim << UserId << ArgDelim
        << "command" << ParamDelim << Command << ArgDelim;
    for (const auto& [k, v] : Params) {
        ss << k << ParamDelim << v << ArgDelim;
    }
    return ss.str();
}

TFtpMessage TFtpMessage::ToMsg(const std::string& msgStr) {
    std::istringstream ss(msgStr);
    std::string token;

    if (!ValidateArgumentsNum(msgStr, 2)) {
        throw std::runtime_error("Couldn't parse: " + msgStr);
    }

    string userId;
    string command;
    std::unordered_map<string, string> params;
    while (std::getline(ss, token, '#')) {
        if (userId.empty()) {
            userId = ParsePair(token).second;
        } else if (command.empty()) {
            command = ParsePair(token).second;
        } else {
            auto [k, v] = ParsePair(token);
            params[k] = v;
        }
    }
    return TFtpMessage(userId, command, std::move(params));
}

void* NUtils::GetInAddr(sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in*)sa)->sin_addr);
    }

    return &(((sockaddr_in6*)sa)->sin6_addr);
}