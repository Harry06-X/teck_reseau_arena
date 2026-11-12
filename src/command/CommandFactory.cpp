/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** CommandFactory
*/

#include "command/CommandFactory.hpp"
#include <algorithm>
#include <cctype>
#include "command/AttackCommand.hpp"
#include "command/AuthCommands.hpp"
#include "command/MoveCommand.hpp"
#include "command/QueryCommands.hpp"

namespace {
    void expectFields(const net::ParsedMessage& msg, std::size_t min, std::size_t max, const std::string& usage)
    {
        if (msg.fields.size() < min || msg.fields.size() > max)
            throw net::ProtocolError("Usage: " + usage);
    }

    void checkCredentials(const std::string& username, const std::string& password)
    {
        if (!net::ProtocolParser::isValidUsername(username))
            throw net::ProtocolError("Username must be 3-16 chars [A-Za-z0-9_]");
        if (!net::ProtocolParser::isValidPassword(password))
            throw net::ProtocolError("Password must be 4-32 printable chars");
    }
}

std::unique_ptr<Command> CommandFactory::create(const net::ParsedMessage& msg, const net::ClientAddress& sender)
{
    const uint32_t id = msg.msgId;

    if (msg.type == "REGISTER") {
        expectFields(msg, 2, 2, "REGISTER|username|password");
        checkCredentials(msg.fields[0], msg.fields[1]);
        return std::make_unique<RegisterCommand>(id, sender, msg.fields[0], msg.fields[1]);
    }
    if (msg.type == "LOGIN") {
        expectFields(msg, 2, 3, "LOGIN|username|password[|WARRIOR/ARCHER]");
        checkCredentials(msg.fields[0], msg.fields[1]);
        Game::PlayerClass playerClass = Game::PlayerClass::Warrior;
        if (msg.fields.size() == 3) {
            if (msg.fields[2] == "ARCHER")
                playerClass = Game::PlayerClass::Archer;
            else if (msg.fields[2] != "WARRIOR")
                throw net::ProtocolError("Class must be WARRIOR or ARCHER");
        }
        return std::make_unique<LoginCommand>(id, sender, msg.fields[0], msg.fields[1], playerClass);
    }
    if (msg.type == "MOVE") {
        expectFields(msg, 1, 1, "MOVE|UP/DOWN/LEFT/RIGHT");
        MovementSystem::Direction dir;
        if (!MovementSystem::parseDirection(msg.fields[0], dir))
            throw net::ProtocolError("Direction must be UP, DOWN, LEFT or RIGHT");
        return std::make_unique<MoveCommand>(id, sender, dir);
    }
    if (msg.type == "ATTACK") {
        expectFields(msg, 1, 1, "ATTACK|player_id");
        const std::string& target = msg.fields[0];
        if (target.empty() || target.size() > 6 || !std::all_of(target.begin(), target.end(), ::isdigit))
            throw net::ProtocolError("player_id must be a number");
        return std::make_unique<AttackCommand>(id, sender, static_cast<ecs::Entity>(std::stoul(target)));
    }
    if (msg.type == "STATUS") {
        expectFields(msg, 0, 0, "STATUS");
        return std::make_unique<StatusCommand>(id, sender);
    }
    if (msg.type == "LOOK") {
        expectFields(msg, 0, 0, "LOOK");
        return std::make_unique<LookCommand>(id, sender);
    }
    if (msg.type == "LOGOUT") {
        expectFields(msg, 0, 0, "LOGOUT");
        return std::make_unique<LogoutCommand>(id, sender);
    }
    if (msg.type == "PING") {
        expectFields(msg, 0, 0, "PING");
        return std::make_unique<PingCommand>(id, sender);
    }
    throw net::ProtocolError("Unknown command " + msg.type);
}
