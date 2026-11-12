/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AuthCommands — REGISTER, LOGIN, LOGOUT
*/

#ifndef AUTHCOMMANDS_HPP_
#define AUTHCOMMANDS_HPP_

#include <string>
#include "Command.hpp"

// REGISTER|username|password
class RegisterCommand : public Command {
public:
    RegisterCommand(uint32_t msgId, const net::ClientAddress& sender, std::string username, std::string password);
    ~RegisterCommand() override = default;

    void execute(GameContext& ctx) override;
    bool requiresLogin() const override { return false; }

private:
    std::string _username;
    std::string _password;
};

// LOGIN|username|password[|WARRIOR/ARCHER]
class LoginCommand : public Command {
public:
    static constexpr std::size_t MAX_PLAYERS = 30;

    LoginCommand(uint32_t msgId, const net::ClientAddress& sender, std::string username,
        std::string password, Game::PlayerClass playerClass);
    ~LoginCommand() override = default;

    void execute(GameContext& ctx) override;
    bool requiresLogin() const override { return false; }

private:
    std::string _username;
    std::string _password;
    Game::PlayerClass _playerClass;
};

// LOGOUT
class LogoutCommand : public Command {
public:
    using Command::Command;
    ~LogoutCommand() override = default;

    void execute(GameContext& ctx) override;
};

#endif /* !AUTHCOMMANDS_HPP_ */
