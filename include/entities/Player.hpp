/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** Player
*/

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "ecs/Entity.hpp"
#include "ecs/ComponentManager.hpp"

/**
 * @brief Classe abstraite représentant un joueur.
 * Fait le pont entre la logique POO (Héritage) et l'ECS (Composition).
 */
class Player {
public:
    // Le constructeur prend l'ID généré par l'EntityManager
    explicit Player(ecs::Entity entityId) : _entityId(entityId) {}
    
    // Destructeur virtuel obligatoire pour l'héritage
    virtual ~Player() = default;

    // Méthode virtuelle pure : rend la classe abstraite
    // Permet d'initialiser les composants spécifiques à chaque classe fille
    virtual void initialize(ecs::ComponentManager& compManager) = 0;

    // Getter pour récupérer l'ID de l'entité ECS
    ecs::Entity getEntityId() const { return _entityId; }

protected:
    // Attribut protégé (Encapsulation)
    ecs::Entity _entityId;
};

#endif /* !PLAYER_HPP_ */