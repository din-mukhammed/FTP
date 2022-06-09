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
        ss << k << ParamDelim << v << ArgDelim;
    }
    return ss.str();
}

TFtpMessage TFtpMessage::ToMsg(const std::string& msgStr) {
    std::istringstream ss(msgStr);
    std::string token;

    std::unordered_map<string, string> params;
    while (std::getline(ss, token, ArgDelim)) {
        auto [k, v] = NUtils::ParsePair(token);
        params[k] = v;
    }
    return TFtpMessage(params);
}


std::string TFtpMessage::GetParam(const std::string& key) const {
    return Params.at(key);
}