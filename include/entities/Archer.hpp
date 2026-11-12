/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** Archer
*/

#ifndef ARCHER_HPP_
#define ARCHER_HPP_

#include "Player.hpp"

// Tireur à distance : attaque jusqu'à 3 cases
class Archer : public Player {
public:
    Archer(ecs::Entity entityId, std::string username) : Player(entityId, std::move(username)) {}
    ~Archer() override = default;

    int getAttackRange() const override { return 3; }
    std::string getClassName() const override { return "ARCHER"; }
};

#endif /* !ARCHER_HPP_ */
