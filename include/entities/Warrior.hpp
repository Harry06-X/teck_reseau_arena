/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** Warrior
*/


#ifndef WARRIOR_HPP_
#define WARRIOR_HPP_

#include "Player.hpp"
#include "components/HealthComponent.hpp"
#include "components/PositionComponent.hpp"

class Warrior : public Player { // Héritage obligatoire
public:
    explicit Warrior(ecs::Entity entityId) : Player(entityId) {}
    ~Warrior() override = default; // Utilisation de override

    // Implémentation du polymorphisme
    void initialize(ecs::ComponentManager& compManager) override {
        // Règles du jeu : 100 HP
        compManager.addComponent(_entityId, HealthComponent{100});
        
        // Position initiale (à déterminer par le Game)
        compManager.addComponent(_entityId, PositionComponent{0, 0});
        
        // Tu pourras ajouter d'autres composants ici (Score, etc.)
    }
};

#endif /* !WARRIOR_HPP_ */