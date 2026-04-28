/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ProtocolParser
*/

#ifndef PROTOCOLPARSER_HPP_
#define PROTOCOLPARSER_HPP_

#include <string>
#include <vector>
#include <memory>
#include "./command/Command.hpp"

namespace net {
    class ProtocolParser {
    public:
        // Prend la string "42|MOVE|UP" et retourne la commande correspondante
        static std::unique_ptr<Command> parse(const std::string& rawMessage);

    private:
        // Helper pour découper la string selon un délimiteur ('|')
        static std::vector<std::string> split(const std::string& s, char delimiter);
    };
}

#endif /* !PROTOCOLPARSER_HPP_ */
