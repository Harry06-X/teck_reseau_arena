/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Command
*/

#include "command/Command.hpp"
#include <iostream>

void Command::run(GameContext& ctx)
{
    net::ClientSession *current = session(ctx);
    if (requiresLogin() && !current) {
        error(ctx, "Not logged in");
        return;
    }
    if (current)
        current->updateLastActivity();

    try {
        execute(ctx);
    } catch (const std::exception& e) {
        std::cerr << "[Command] " << _sender.toString() << " : " << e.what() << std::endl;
        error(ctx, "Internal server error");
    }
}

void Command::reply(GameContext& ctx, const std::string& type, const std::vector<std::string>& fields) const
{
    ctx.responder.reply(_sender, type, fields);
}

void Command::ok(GameContext& ctx, const std::string& message) const
{
    reply(ctx, "OK", {message});
}

void Command::error(GameContext& ctx, const std::string& message) const
{
    reply(ctx, "ERROR", {message});
}

void Command::sendState(GameContext& ctx, ecs::Entity entity)
{
    net::ClientSession *owner = ctx.sessions.findByEntity(entity);
    if (!owner || !ctx.game.hasPlayer(entity))
        return;
    PlayerState s = ctx.game.getState(entity);
    ctx.responder.reply(owner->getAddress(), "STATE", {
        std::to_string(s.hp), std::to_string(s.x), std::to_string(s.y), std::to_string(s.score)
    });
}

net::ClientSession *Command::session(GameContext& ctx) const
{
    return ctx.sessions.find(_sender);
}
