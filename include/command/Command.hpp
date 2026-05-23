/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Command
*/

/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Command — Classe abstraite (Pattern Command)
*/

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include "ecs/Entity.hpp"
#include "ecs/ComponentManager.hpp"

class Command {
public:
    // Constructeur : toute commande connaît l'entité concernée
    // et a accès au ComponentManager pour lire/modifier les composants.
    explicit Command(ecs::Entity playerId, ecs::ComponentManager& compManager)
        : _playerId(playerId), _compManager(compManager) {}

    // Destructeur virtuel obligatoire pour le polymorphisme
    virtual ~Command() = default;

    // Méthode virtuelle pure : rend la classe abstraite.
    // Chaque sous-classe DOIT implémenter son propre execute().
    virtual void execute() = 0;

    // Getter utile pour les logs ou le débogage
    ecs::Entity getPlayerId() const { return _playerId; }

protected:
    ecs::Entity _playerId;      // L'entité ECS du joueur concerné
    ecs::ComponentManager& _compManager;   // Référence vers le gestionnaire de composants
};

#endif /* !COMMAND_HPP_ */