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
private:
    std::unordered_map<std::string, std::string> Params;

};
} // NUtils