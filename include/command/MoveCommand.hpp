/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** MoveCommand — MOVE|UP/DOWN/LEFT/RIGHT
*/

#ifndef MOVECOMMAND_HPP_
#define MOVECOMMAND_HPP_

#include "Command.hpp"
#include "systems/MovementSystem.hpp"

class MoveCommand : public Command {
public:
    MoveCommand(uint32_t msgId, const net::ClientAddress& sender, MovementSystem::Direction dir);
    ~MoveCommand() override = default;

    void execute(GameContext& ctx) override;

private:
    MovementSystem::Direction _direction;
};

#endif /* !MOVECOMMAND_HPP_ */
