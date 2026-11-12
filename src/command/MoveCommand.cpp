/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** MoveCommand
*/

#include "command/MoveCommand.hpp"

MoveCommand::MoveCommand(uint32_t msgId, const net::ClientAddress& sender, MovementSystem::Direction dir)
    : Command(msgId, sender), _direction(dir)
{}

void MoveCommand::execute(GameContext& ctx)
{
    ecs::Entity me = session(ctx)->getEntityId();

    if (!ctx.game.isAlive(me)) {
        error(ctx, "You are dead, respawn in " + std::to_string(ctx.game.respawnIn(me)) + "s");
        return;
    }
    // Délégation au MovementSystem (bornes 0..19 gérées par le système)
    if (!ctx.game.move(me, _direction))
        error(ctx, "Blocked by the arena border");
    sendState(ctx, me);
}
