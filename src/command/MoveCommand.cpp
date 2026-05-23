/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** MoveCommand
*/


#include "command/MoveCommand.hpp"
#include <iostream>

MoveCommand::MoveCommand(ecs::Entity entityId,
                         MovementSystem::Direction dir,
                         ecs::ComponentManager& compManager)
    : Command(entityId, compManager)   // Appel du constructeur de Command
    , _direction(dir)                  // Stockage de la direction
{}

void MoveCommand::execute()
{
    std::cout << "[MoveCommand] execute() — entity " << _playerId << "\n";

    // Délégation au MovementSystem
    bool moved = _movSystem.update(_playerId, _direction, _compManager);

    if (!moved) {
        // Le mouvement était invalide (bord de carte) : on l'ignore silencieusement.
        // Le serveur peut choisir d'envoyer un ERROR au client ici si besoin.
        std::cout << "[MoveCommand] Mouvement bloqué pour entity " << _playerId << "\n";
    }
}