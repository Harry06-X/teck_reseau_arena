#pragma once

class MovementSystem {
public:
    MovementSystem();
    ~MovementSystem();
};


#pragma once

#include <string>
#include <iostream>
#include "core/ISystem.hpp"
#include "ecs/Entity.hpp"
#include "ecs/ComponentManager.hpp"
#include "components/PositionComponent.hpp"

/*
 * MovementSystem applique la règle de déplacement du jeu :
 *   - Modifie le PositionComponent de l'entité selon la direction
 *   - Bloque le mouvement si la nouvelle position sort de [0, 19]
 *   - Ne sait rien du réseau : il reçoit juste un entityId et une direction
 *
 * Il est appelé par MoveCommand::execute().
 */
class MovementSystem : public ISystem {
public:
    // Les 4 directions possibles (correspond aux valeurs du protocole)
    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    MovementSystem() = default;
    ~MovementSystem() override = default;

    // Convertit la chaîne du protocole ("UP", "DOWN"...) en enum Direction.
    // Retourne false si la direction est invalide (protection contre les paquets malformés).
    static bool parseDirection(const std::string& raw, Direction& outDir);

    // Déplace l'entité dans la direction donnée.
    // Retourne true si le mouvement a été appliqué, false si bloqué par les bords.
    bool update(ecs::Entity entityId, Direction dir, ecs::ComponentManager& compManager);

private:
    // Taille de la grille (les positions valides sont [0, GRID_SIZE - 1])
    static constexpr int GRID_SIZE = 20;
};