#pragma once
#include <bits/stdc++.h>
#include <netinet/in.h>


namespace NUtils {

const char ParamDelim = '=';
const char ArgDelim = '#';

class TMessage {
public:
    TMessage() = default;
    
    TMessage(const std::string& userId);

    virtual std::string ToStr() const;

    static TMessage ToMsg(const std::string& msgStr);
protected:
    static std::pair<std::string, std::string> ParsePair(const std::string& str);

    const std::string UserId;

    static bool ValidateArgumentsNum(const std::string& msgStr, int expectedArgsNum);
};


class TFtpMessage : public TMessage {
public:
    TFtpMessage() = default;

    TFtpMessage(
        const std::string& userId,
        const std::string& command,
        std::unordered_map<std::string, std::string>&& params);

    std::string ToStr() const override;

    static TFtpMessage ToMsg(const std::string& msgStr);
private:
    const std::string Command;
    std::unordered_map<std::string, std::string> Params;
};


void* GetInAddr(sockaddr* sa);

} // NUtils