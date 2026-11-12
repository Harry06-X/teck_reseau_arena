/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AttackCommand — ATTACK|player_id
*/

#ifndef ATTACKCOMMAND_HPP_
#define ATTACKCOMMAND_HPP_

#include "Command.hpp"

class AttackCommand : public Command {
public:
    AttackCommand(uint32_t msgId, const net::ClientAddress& sender, ecs::Entity target);
    ~AttackCommand() override = default;

    void execute(GameContext& ctx) override;

private:
    ecs::Entity _target;
};

#endif /* !ATTACKCOMMAND_HPP_ */
