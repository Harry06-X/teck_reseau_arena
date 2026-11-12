/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** Warrior
*/

#ifndef WARRIOR_HPP_
#define WARRIOR_HPP_

#include "Player.hpp"

// Combattant au corps à corps : n'attaque que les cases adjacentes
class Warrior : public Player {
public:
    Warrior(ecs::Entity entityId, std::string username) : Player(entityId, std::move(username)) {}
    ~Warrior() override = default;

    int getAttackRange() const override { return 1; }
    std::string getClassName() const override { return "WARRIOR"; }
};

#endif /* !WARRIOR_HPP_ */
