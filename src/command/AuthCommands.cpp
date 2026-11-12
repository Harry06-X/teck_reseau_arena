/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AuthCommands
*/

#include "command/AuthCommands.hpp"
#include <iostream>

RegisterCommand::RegisterCommand(uint32_t msgId, const net::ClientAddress& sender,
    std::string username, std::string password)
    : Command(msgId, sender), _username(std::move(username)), _password(std::move(password))
{}

void RegisterCommand::execute(GameContext& ctx)
{
    if (ctx.accounts.registerUser(_username, _password) == AccountManager::Result::AlreadyExists) {
        error(ctx, "Username already taken");
        return;
    }
    std::cout << "[Auth] New account '" << _username << "' from " << _sender.toString() << std::endl;
    ok(ctx, "Account created, you can now LOGIN");
}

LoginCommand::LoginCommand(uint32_t msgId, const net::ClientAddress& sender, std::string username,
    std::string password, Game::PlayerClass playerClass)
    : Command(msgId, sender), _username(std::move(username)), _password(std::move(password)),
      _playerClass(playerClass)
{}

void LoginCommand::execute(GameContext& ctx)
{
    if (net::ClientSession *current = session(ctx)) {
        error(ctx, "Already logged in as " + current->getUsername());
        return;
    }

    switch (ctx.accounts.authenticate(_username, _password, _sender.ip)) {
        case AccountManager::Result::TooManyAttempts:
            error(ctx, "Too many failed attempts, try again in a minute");
            return;
        case AccountManager::Result::UnknownUser:
        case AccountManager::Result::WrongPassword:
            // Message volontairement identique : ne révèle pas si le compte existe
            error(ctx, "Invalid username or password");
            return;
        default:
            break;
    }

    if (ctx.sessions.findByUsername(_username)) {
        error(ctx, "This account is already connected");
        return;
    }
    if (ctx.sessions.addresses().size() >= MAX_PLAYERS) {
        error(ctx, "Arena is full");
        return;
    }

    ecs::Entity id = ctx.game.spawnPlayer(_username, _playerClass);
    ctx.sessions.open(_sender, _username, id);
    const std::string className = ctx.game.getPlayer(id).getClassName();
    std::cout << "[Auth] " << _username << " joined as player " << id << " (" << className << ") from "
              << _sender.toString() << std::endl;
    ok(ctx, "Welcome " + _username + ", you are player " + std::to_string(id) + " (" + className + ")");
    sendState(ctx, id);
}

void LogoutCommand::execute(GameContext& ctx)
{
    net::ClientSession *current = session(ctx);
    std::cout << "[Auth] " << current->getUsername() << " logged out" << std::endl;
    ctx.game.removePlayer(current->getEntityId());
    ok(ctx, "Goodbye " + current->getUsername());
    ctx.sessions.close(_sender);
}
