/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** RespawnSystem
*/

#include "systems/RespawnSystem.hpp"

void RespawnSystem::handleDeath(ecs::Entity entity)
{
    _deaths[entity] = Clock::now();
}

std::vector<ecs::Entity> RespawnSystem::update()
{
    std::vector<ecs::Entity> ready;
    const auto now = Clock::now();

    for (auto it = _deaths.begin(); it != _deaths.end(); ) {
        if (now - it->second >= DELAY) {
            ready.push_back(it->first);
            it = _deaths.erase(it);
        } else {
            ++it;
        }
    }
    return ready;
}

int RespawnSystem::secondsLeft(ecs::Entity entity) const
{
    auto it = _deaths.find(entity);
    if (it == _deaths.end())
        return 0;
    auto left = std::chrono::duration_cast<std::chrono::seconds>(DELAY - (Clock::now() - it->second));
    return left.count() > 0 ? static_cast<int>(left.count()) + 1 : 1;
}

void RespawnSystem::forget(ecs::Entity entity)
{
    _deaths.erase(entity);
}
