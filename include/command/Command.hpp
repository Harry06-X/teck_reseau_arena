/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Command
*/

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include "../ecs/Entity.hpp"
#include "../ecs/ComponentManager.hpp"

class Command {
public:
    explicit Command(ecs::Entity playerId, ecs::ComponentManager& compManager)
        : _playerId(playerId), _compManager(compManager) {}

    virtual ~Command() = default;

    virtual void execute() = 0;

    ecs::Entity getPlayerId() const { return _playerId; }

protected:
    ecs::Entity _playerId;      // L'entité ECS du joueur concerné
    ecs::ComponentManager& _compManager;   // Référence vers le gestionnaire de composants
};

#endif /* !COMMAND_HPP_ */