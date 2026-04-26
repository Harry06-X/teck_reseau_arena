/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ProtocolParser
*/

#include "../include/network/ProtocolParser.hpp"
#include "../include/command/MoveCommand.hpp"
#include "../include/command/AttackCommand.hpp"
#include <sstream>
#include <memory>


namespace net {

    std::vector<std::string> ProtocolParser::split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::unique_ptr<Command> ProtocolParser::parse(const std::string& rawMessage) {
        auto tokens = split(rawMessage, '|');

        // Format attendu : MSGID | TYPE | DATA
        if (tokens.size() < 3) return nullptr;

        std::size_t msgId = std::stoul(tokens[0]);
        std::string type = tokens[1];
        std::string data = tokens[2];

        if (type == "MOVE") {
            // On crée la commande de mouvement (ton binôme remplira le execute())
            return std::make_unique<MoveCommand>(msgId, data);
        } else if (type == "ATTACK") {
            return std::make_unique<AttackCommand>(msgId, data);
        }

        return nullptr;
    }
}