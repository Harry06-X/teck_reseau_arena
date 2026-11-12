/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** CombatSystem — règles d'attaque
*/

#ifndef COMBATSYSTEM_HPP_
#define COMBATSYSTEM_HPP_

#include "core/ISystem.hpp"
#include "ecs/ComponentManager.hpp"
#include "components/PositionComponent.hpp"
#include "entities/Player.hpp"

/*
 * CombatSystem applique les règles du cahier des charges :
 *   - une attaque inflige 20 HP
 *   - HP <= 0 → joueur mort, +1 au score de l'attaquant
 *   - portée selon la classe (polymorphisme Player::getAttackRange)
 */
class CombatSystem : public ISystem {
public:
    static constexpr int DAMAGE = 20;

    enum class Outcome {
        Hit,
        Killed,
        OutOfRange,
        TargetDead,
        AttackerDead
    };

    CombatSystem() = default;
    ~CombatSystem() override = default;

    Outcome attack(const Player& attacker, const Player& target, ecs::ComponentManager& compManager);

    // Distance de Chebyshev : les diagonales comptent comme 1 case
    static bool isInRange(const PositionComponent& from, const PositionComponent& to, int range);
};

#endif /* !COMBATSYSTEM_HPP_ */
