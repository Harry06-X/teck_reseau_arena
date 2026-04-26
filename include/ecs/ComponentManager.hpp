/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** ComponentManager
*/

#ifndef COMPONENTMANAGER_HPP_
#define COMPONENTMANAGER_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>
#include "Entity.hpp"
#include "../core/IComponent.hpp"

namespace ecs {

    // Interface pour stocker des tableaux de composants de n'importe quel type
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void entityDestroyed(Entity entity) = 0;
    };

    class ComponentManager {
    public:
        ComponentManager();
        ~ComponentManager() = default;

        // Déclarations des méthodes templates
        template<typename T>
        void registerComponent();

        template<typename T>
        void addComponent(Entity entity, T component);

        template<typename T>
        void removeComponent(Entity entity);

        template<typename T>
        T& getComponent(Entity entity);

        void entityDestroyed(Entity entity);

    private:
        // Associe un type de composant à son tableau de stockage
        std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> _componentArrays;

        // Méthode interne pour récupérer le tableau du bon type
        template<typename T>
        std::shared_ptr<IComponentArray> getComponentArray();
    };
}

// Note : En C++, les définitions des templates doivent normalement être visibles.
// Pour respecter ta règle, on inclut un fichier de définitions à la fin, 
// ou on accepte une exception pour les headers de templates.
// Ici, nous allons placer les définitions dans un fichier séparé .tpp 
// ou les laisser dans le header si tu l'autorises pour les templates.
#include "ComponentManager.tpp" 

#endif /* !COMPONENTMANAGER_HPP_ */