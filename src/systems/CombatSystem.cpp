/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** CombatSystem
*/

#include "systems/CombatSystem.hpp"
#include <algorithm>
#include <cstdlib>

CombatSystem::Outcome CombatSystem::attack(const Player& attacker, const Player& target,
    ecs::ComponentManager& compManager)
{
    auto& attackerHp = compManager.getComponent<HealthComponent>(attacker.getEntityId());
    auto& targetHp = compManager.getComponent<HealthComponent>(target.getEntityId());

    if (attackerHp.hp <= 0)
        return Outcome::AttackerDead;
    if (targetHp.hp <= 0)
        return Outcome::TargetDead;

    const auto& from = compManager.getComponent<PositionComponent>(attacker.getEntityId());
    const auto& to = compManager.getComponent<PositionComponent>(target.getEntityId());
    if (!isInRange(from, to, attacker.getAttackRange()))
        return Outcome::OutOfRange;

    targetHp.hp = std::max(0, targetHp.hp - DAMAGE);
    if (targetHp.hp > 0)
        return Outcome::Hit;

    compManager.getComponent<ScoreComponent>(attacker.getEntityId()).score += 1;
    return Outcome::Killed;
}

bool CombatSystem::isInRange(const PositionComponent& from, const PositionComponent& to, int range)
{
    return std::max(std::abs(from.x - to.x), std::abs(from.y - to.y)) <= range;
}
