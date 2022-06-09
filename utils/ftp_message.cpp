#include "ftp_message.h"

using namespace NUtils;
using std::string;

TFtpMessage::TFtpMessage(const std::unordered_map<std::string, std::string>& params)
: Params(params)
{
}

std::string TFtpMessage::ToStr() const {
    std::stringstream ss;
    for (const auto& [k, v] : Params) {
        ss << k << PairDelim << v << ArgDelim;
    }
    return ss.str();
}

TFtpMessage TFtpMessage::ToMsg(const std::string& msgStr) {
    std::istringstream ss(msgStr);
    std::string token;

    std::unordered_map<string, string> params;
    while (std::getline(ss, token, ArgDelim)) {
        auto [k, v] = ParsePair(token);
        params[k] = v;
    }
    return TFtpMessage(params);
}

bool TFtpMessage::Send(int sockFd) const {
    auto msgStr = ToStr();
    printf("Sending TFtpMessage: size = %zu, content: %s\n", msgStr.size(), msgStr.c_str());
    if (send(sockFd, msgStr.c_str(), msgStr.size(), 0) < 0) {
        perror("send TFtpMessage");
        return false;
    }
    return true;
}

bool TFtpMessage::RecvMsg(int sockFd, int maxDataSize, TFtpMessage* msg) {
    assert(msg);
    std::vector<char> buf(maxDataSize);
    int numbytes = 0;

    if (numbytes = recv(sockFd, &buf[0], maxDataSize, 0); numbytes < 0) {
        perror("Couldn't recv TFtpMsg");
        return false;
    }
    *msg = NUtils::TFtpMessage::ToMsg(buf.data());
    auto msgStr = msg->ToStr();
    printf("Received Message: size = %zu, content: %s\n", msgStr.size(), msgStr.c_str());
    return true;
}

std::string TFtpMessage::GetParam(const std::string& key) const {
    return Params.at(key);
}

std::pair<std::string, std::string> TFtpMessage::ParsePair(const std::string& pairStr) {
    std::pair<std::string, std::string> ans;
    int delimPos = pairStr.find(PairDelim);
    ans.first = pairStr.substr(0, delimPos);
    ans.second = pairStr.substr(delimPos + 1);
    return ans;
}

std::vector<std::string> TFtpMessage::ParseList(const std::string& listStr) {
    std::vector<std::string> res;
    std::istringstream ss(listStr);
    std::string token;

    std::unordered_map<string, string> params;
    while (std::getline(ss, token, ListDelim)) {
        res.emplace_back(std::move(token));
    }
    return res;
}
