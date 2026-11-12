/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ProtocolParser
*/

#include "network/ProtocolParser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace net {

    std::vector<std::string> ProtocolParser::split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
            tokens.push_back(token);
        return tokens;
    }

    bool ProtocolParser::isPrintable(const std::string& s)
    {
        return std::all_of(s.begin(), s.end(), [](unsigned char c) {
            return c >= 0x20 && c < 0x7f;
        });
    }

    ParsedMessage ProtocolParser::parse(const std::string& raw)
    {
        std::string data = raw;
        // Tolère un retour à la ligne final (clients tapés à la main avec netcat)
        while (!data.empty() && (data.back() == '\n' || data.back() == '\r'))
            data.pop_back();

        if (data.empty())
            throw ProtocolError("Empty message");
        if (data.size() > MAX_DATAGRAM)
            throw ProtocolError("Message too long");
        if (!isPrintable(data))
            throw ProtocolError("Invalid characters in message");

        auto tokens = split(data, SEPARATOR);
        if (tokens.size() < 2)
            throw ProtocolError("Expected format MSGID|TYPE|FIELDS");

        const std::string& id = tokens[0];
        if (id.empty() || id.size() > 9 || !std::all_of(id.begin(), id.end(), ::isdigit))
            throw ProtocolError("Invalid message id");

        ParsedMessage msg;
        msg.msgId = static_cast<uint32_t>(std::stoul(id));
        msg.type = tokens[1];
        if (msg.type.empty() || msg.type.size() > 16
            || !std::all_of(msg.type.begin(), msg.type.end(), [](char c) { return std::isupper(c) || c == '_'; }))
            throw ProtocolError("Invalid message type");

        const std::size_t limit = msg.type == "GAME_UPDATE" ? MAX_PAYLOAD_FIELD : MAX_FIELD;
        for (std::size_t i = 2; i < tokens.size(); ++i) {
            if (tokens[i].size() > limit)
                throw ProtocolError("Field too long");
            msg.fields.push_back(tokens[i]);
        }
        return msg;
    }

    std::string ProtocolParser::serialize(uint32_t msgId, const std::string& type,
        const std::vector<std::string>& fields)
    {
        std::string out = std::to_string(msgId) + SEPARATOR + type;
        for (const auto& field : fields) {
            std::string clean = field;
            std::replace_if(clean.begin(), clean.end(), [](unsigned char c) {
                return c == SEPARATOR || c < 0x20 || c >= 0x7f;
            }, ' ');
            out += SEPARATOR + clean;
        }
        return out;
    }

    bool ProtocolParser::isValidUsername(const std::string& name)
    {
        return name.size() >= 3 && name.size() <= 16
            && std::all_of(name.begin(), name.end(), [](unsigned char c) { return std::isalnum(c) || c == '_'; });
    }

    bool ProtocolParser::isValidPassword(const std::string& password)
    {
        return password.size() >= 4 && password.size() <= 32 && isPrintable(password)
            && password.find(SEPARATOR) == std::string::npos;
    }
}
