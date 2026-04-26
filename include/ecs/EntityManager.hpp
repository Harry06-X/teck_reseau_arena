/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** EntityManager
*/

#ifndef ENTITY_MANAGER_HPP_
#define ENTITY_MANAGER_HPP_

#include <vector>
#include <queue>
#include <unordered_set>
#include "Entity.hpp"

namespace ecs {
    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;

        Entity createEntity();
        void destroyEntity(Entity entity);
        const std::unordered_set<Entity>& getLivingEntities() const;

    private:
        std::queue<Entity> _availableIds;
        std::unordered_set<Entity> _livingEntities;
        static constexpr std::size_t MAX_ENTITIES = 1000;
    };
}

#endif /* !ENTITYMANAGER_HPP_ */
