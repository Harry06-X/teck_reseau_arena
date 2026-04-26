/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** Archer
*/


#ifndef ARCHER_HPP_
#define ARCHER_HPP_

#include "Player.hpp"
#include "components/HealthComponent.hpp"
#include "components/PositionComponent.hpp"

class Archer : public Player { // Héritage obligatoire
public:
    explicit Archer(ecs::Entity entityId) : Player(entityId) {}
    ~Archer() override = default; // Utilisation de override

    // Implémentation du polymorphisme
    void initialize(ecs::ComponentManager& compManager) override {
        // Règles du jeu : 100 HP
        compManager.addComponent(_entityId, HealthComponent{100});
        
        // Position initiale (à déterminer par le Game)
        compManager.addComponent(_entityId, PositionComponent{0, 0});
        
        // Tu pourras ajouter d'autres composants ici (Score, etc.)
    }
};

#endif /* !Archer_HPP_ */