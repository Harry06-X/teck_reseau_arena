/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** MoveCommand — Commande de déplacement (Pattern Command)
*/

#ifndef MOVECOMMAND_HPP_
#define MOVECOMMAND_HPP_

#include "Command.hpp"
#include "../systems/MovementSystem.hpp"

class MoveCommand : public Command {
public:
    MoveCommand(ecs::Entity entityId, MovementSystem::Direction dir, ecs::ComponentManager& compManager);

    ~MoveCommand() override = default;

    void execute() override;

private:
    MovementSystem::Direction _direction;   // Direction demandée
    MovementSystem _movSystem;   // Instance locale du système
};

#endif /* !MOVECOMMAND_HPP_ */