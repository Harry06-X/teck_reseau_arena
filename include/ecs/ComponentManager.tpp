/*
** EPITECH PROJECT, 2026
** arena_command
** File description:
** ComponentManager
*/

#ifndef COMPONENT_MANAGER_TPP_
#define COMPONENT_MANAGER_TPP_

#include <stdexcept>
#include "ComponentManager.hpp"

namespace ecs {

    // Helper : Classe interne pour stocker les composants d'un type précis T
    template<typename T>
    class ComponentArray : public IComponentArray {
    public:
        void insertData(Entity entity, T component) {
            _entityToComponentMap[entity] = component;
        }

        void removeData(Entity entity) {
            _entityToComponentMap.erase(entity);
        }

        T& getData(Entity entity) {
            if (_entityToComponentMap.find(entity) == _entityToComponentMap.end()) {
                throw std::runtime_error("Component not found for entity.");
            }
            return _entityToComponentMap[entity];
        }

        void entityDestroyed(Entity entity) override {
            removeData(entity);
        }

    private:
        // Associe un ID d'entité à son instance de composant
        std::unordered_map<Entity, T> _entityToComponentMap;
    };

    // Implémentation des méthodes du ComponentManager
    template<typename T>
    void ComponentManager::registerComponent() {
        std::type_index typeName = std::type_index(typeid(T));
        if (_componentArrays.find(typeName) == _componentArrays.end()) {
            _componentArrays[typeName] = std::make_shared<ComponentArray<T>>();
        }
    }

    template<typename T>
    std::shared_ptr<IComponentArray> ComponentManager::getComponentArray() {
        std::type_index typeName = std::type_index(typeid(T));
        if (_componentArrays.find(typeName) == _componentArrays.end()) {
            throw std::runtime_error("Component not registered before use.");
        }
        return _componentArrays[typeName];
    }

    template<typename T>
    void ComponentManager::addComponent(Entity entity, T component) {
        std::static_pointer_cast<ComponentArray<T>>(getComponentArray<T>())->insertData(entity, component);
    }

    template<typename T>
    void ComponentManager::removeComponent(Entity entity) {
        std::static_pointer_cast<ComponentArray<T>>(getComponentArray<T>())->removeData(entity);
    }

    template<typename T>
    T& ComponentManager::getComponent(Entity entity) {
        return std::static_pointer_cast<ComponentArray<T>>(getComponentArray<T>())->getData(entity);
    }
}

#endif