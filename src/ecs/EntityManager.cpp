/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** EntityManager
*/

#include "ecs/EntityManager.hpp"

namespace ecs {

    EntityManager::EntityManager()
    {
        // Initialisation du pool d'identifiants
        for (Entity i = 0; i < MAX_ENTITIES; ++i) {
            _availableIds.push(i);
        }
    }

    Entity EntityManager::createEntity()
    {
        if (_availableIds.empty()) {
            // Optionnel : Gérer l'erreur si plus d'IDs dispos
            return 0; 
        }

        Entity id = _availableIds.front();
        _availableIds.pop();
        _livingEntities.insert(id);
        return id;
    }

    void EntityManager::destroyEntity(Entity entity)
    {
        if (_livingEntities.find(entity) != _livingEntities.end()) {
            _livingEntities.erase(entity);
            _availableIds.push(entity);
        }
    }

    const std::unordered_set<Entity>& EntityManager::getLivingEntities() const
    {
        return _livingEntities;
    }

}