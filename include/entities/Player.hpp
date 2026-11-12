/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** Player
*/

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <string>
#include "ecs/Entity.hpp"
#include "ecs/ComponentManager.hpp"
#include "components/HealthComponent.hpp"
#include "components/PositionComponent.hpp"
#include "components/ScoreComponent.hpp"

/**
 * @brief Classe abstraite représentant un joueur.
 * Fait le pont entre la logique POO (Héritage) et l'ECS (Composition).
 */
class Player {
public:
    Player(ecs::Entity entityId, std::string username)
        : _entityId(entityId), _username(std::move(username)) {}

    // Destructeur virtuel obligatoire pour l'héritage
    virtual ~Player() = default;

    // Ajoute les composants de départ : 100 HP, position, score à 0
    virtual void initialize(ecs::ComponentManager& compManager, int x, int y)
    {
        compManager.addComponent(_entityId, HealthComponent{STARTING_HP});
        compManager.addComponent(_entityId, PositionComponent{x, y});
        compManager.addComponent(_entityId, ScoreComponent{0});
    }

    // Méthodes virtuelles pures : rendent la classe abstraite
    virtual int getAttackRange() const = 0;
    virtual std::string getClassName() const = 0;

    ecs::Entity getEntityId() const { return _entityId; }
    const std::string& getUsername() const { return _username; }

    static constexpr int STARTING_HP = 100;

protected:
    ecs::Entity _entityId;
    std::string _username;
};

#endif /* !PLAYER_HPP_ */
