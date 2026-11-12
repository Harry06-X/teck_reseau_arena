/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** CommandFactory — ParsedMessage → Command concrète
*/

#ifndef COMMANDFACTORY_HPP_
#define COMMANDFACTORY_HPP_

#include <memory>
#include "command/Command.hpp"
#include "network/ProtocolParser.hpp"

class CommandFactory {
public:
    // Valide les champs et construit la commande — lève net::ProtocolError
    static std::unique_ptr<Command> create(const net::ParsedMessage& msg, const net::ClientAddress& sender);
};

#endif /* !COMMANDFACTORY_HPP_ */
