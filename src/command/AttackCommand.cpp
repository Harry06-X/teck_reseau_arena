/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AttackCommand
*/

#include "command/AttackCommand.hpp"

AttackCommand::AttackCommand(uint32_t msgId, const net::ClientAddress& sender, ecs::Entity target)
    : Command(msgId, sender), _target(target)
{}

void AttackCommand::execute(GameContext& ctx)
{
    ecs::Entity me = session(ctx)->getEntityId();

    if (!ctx.game.hasPlayer(_target)) {
        error(ctx, "Unknown player " + std::to_string(_target));
        return;
    }
    if (_target == me) {
        error(ctx, "You cannot attack yourself");
        return;
    }

    const std::string targetName = ctx.game.getPlayer(_target).getUsername();
    switch (ctx.game.attack(me, _target)) {
        case CombatSystem::Outcome::AttackerDead:
            error(ctx, "You are dead, respawn in " + std::to_string(ctx.game.respawnIn(me)) + "s");
            return;
        case CombatSystem::Outcome::TargetDead:
            error(ctx, targetName + " is already dead");
            return;
        case CombatSystem::Outcome::OutOfRange:
            error(ctx, targetName + " is out of range (your range: "
                + std::to_string(ctx.game.getPlayer(me).getAttackRange()) + ")");
            return;
        case CombatSystem::Outcome::Hit:
            ok(ctx, "Hit " + targetName + " (" + std::to_string(ctx.game.getState(_target).hp) + " HP left)");
            break;
        case CombatSystem::Outcome::Killed:
            ctx.responder.broadcast("PLAYER_KILLED", {std::to_string(_target)});
            ok(ctx, "You eliminated " + targetName);
            break;
    }
    sendState(ctx, me);
    sendState(ctx, _target);
}
