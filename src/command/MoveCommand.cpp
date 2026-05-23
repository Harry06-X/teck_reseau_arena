/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** MoveCommand
*/


#include "command/MoveCommand.hpp"
#include <iostream>

// -----------------------------------------------------------------------------
// Constructeur
// -----------------------------------------------------------------------------
// On stocke la direction et on passe entityId + compManager à la classe mère.
// La direction est déjà un enum — le parsing "UP" → Direction::UP
// a été fait par ton binôme dans ProtocolParser avant de créer cet objet.
// -----------------------------------------------------------------------------
MoveCommand::MoveCommand(ecs::Entity entityId,
                         MovementSystem::Direction dir,
                         ecs::ComponentManager& compManager)
    : Command(entityId, compManager)   // Appel du constructeur de Command
    , _direction(dir)                  // Stockage de la direction
{
    // Rien d'autre à faire ici : tout est dans execute()
}

// -----------------------------------------------------------------------------
// execute()
// -----------------------------------------------------------------------------
// Appelée par la boucle de jeu (ton code) quand elle dépile la CommandQueue.
//
// Séquence :
//   1. On délègue au MovementSystem qui modifie le PositionComponent
//   2. Si le mouvement est bloqué (bord de grille), on log simplement
//      Sans planter : le jeu continue, le joueur reste au même endroit.
//
// Exemple de flux complet :
//   Réseau reçoit "42|MOVE|UP"
//   → ton binôme crée new MoveCommand(entityId, Direction::UP, compManager)
//   → ton binôme fait commandQueue.push(std::move(cmd))
//   → ta boucle de jeu fait cmd->execute()
//   → ici, _movSystem.update(entityId, UP, compManager) modifie pos.y -= 1
// -----------------------------------------------------------------------------
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