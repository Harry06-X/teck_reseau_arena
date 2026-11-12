/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** QueryCommands
*/

#include "command/QueryCommands.hpp"

void StatusCommand::execute(GameContext& ctx)
{
    sendState(ctx, session(ctx)->getEntityId());
}

void LookCommand::execute(GameContext& ctx)
{
    reply(ctx, "GAME_UPDATE", {ctx.game.serialize()});
}
