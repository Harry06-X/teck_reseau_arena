/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** ComponentManager
*/

#include "../../include/ecs/ComponentManager.hpp"

#include "ecs/ComponentManager.hpp"

namespace ecs {

    ComponentManager::ComponentManager() {
        // Initialisation si nécessaire
    }

    void ComponentManager::entityDestroyed(Entity entity) {
        // Informe tous les tableaux qu'une entité a été supprimée 
        // pour nettoyer les composants associés
        for (auto const& pair : _componentArrays) {
            auto const& componentArray = pair.second;
            componentArray->entityDestroyed(entity);
        }
    }

}