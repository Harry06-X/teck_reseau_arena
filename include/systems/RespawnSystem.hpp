/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** RespawnSystem — réapparition automatique après 10 secondes
*/

#ifndef RESPAWNSYSTEM_HPP_
#define RESPAWNSYSTEM_HPP_

#include <chrono>
#include <map>
#include <vector>
#include "core/ISystem.hpp"
#include "ecs/Entity.hpp"

class RespawnSystem : public ISystem {
public:
    using Clock = std::chrono::steady_clock;
    static constexpr std::chrono::seconds DELAY{10};

    RespawnSystem() = default;
    ~RespawnSystem() override = default;

    // Démarre le compte à rebours d'un joueur mort
    void handleDeath(ecs::Entity entity);

    // Retourne les joueurs dont le délai est écoulé (et les retire de la liste)
    std::vector<ecs::Entity> update();

    // Secondes restantes avant réapparition (0 si vivant)
    int secondsLeft(ecs::Entity entity) const;

    void forget(ecs::Entity entity);

private:
    std::map<ecs::Entity, Clock::time_point> _deaths;
};

#endif /* !RESPAWNSYSTEM_HPP_ */
