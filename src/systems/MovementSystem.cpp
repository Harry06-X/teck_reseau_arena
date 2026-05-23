#include "../../include/systems/MovementSystem.hpp"

MovementSystem::MovementSystem() {}

MovementSystem::~MovementSystem() {}
// -----------------------------------------------------------------------------
// parseDirection
// -----------------------------------------------------------------------------
// Convertit la chaîne brute du protocole en valeur d'enum.
// Exemple : "UP" → Direction::UP
// Si la valeur est inconnue, on retourne false pour que MoveCommand
// puisse logger l'erreur et ignorer le paquet sans crasher.
// -----------------------------------------------------------------------------
bool MovementSystem::parseDirection(const std::string& raw, Direction& outDir)
{
    if (raw == "UP") {
        outDir = Direction::UP;
        return true;
    }
    if (raw == "DOWN") {
        outDir = Direction::DOWN;
        return true;
    }
    if (raw == "LEFT") {
        outDir = Direction::LEFT;
        return true;
    }
    if (raw == "RIGHT") {
        outDir = Direction::RIGHT;
        return true;
    }
    // Direction inconnue : paquet malformé côté client
    std::cerr << "[MovementSystem] Direction invalide : \"" << raw << "\"\n";
    return false;
}

// -----------------------------------------------------------------------------
// update
// -----------------------------------------------------------------------------
// Applique le déplacement d'une case dans la direction demandée.
//
// La grille fait 20x20, donc les coordonnées valides sont [0, 19].
// Si le joueur est déjà au bord et essaie de sortir → on bloque,
// on retourne false pour informer MoveCommand qu'aucun mouvement n'a eu lieu.
//
// Axes :
//   UP    → y diminue  (ligne du haut = y=0)
//   DOWN  → y augmente (ligne du bas  = y=19)
//   LEFT  → x diminue
//   RIGHT → x augmente
// -----------------------------------------------------------------------------
bool MovementSystem::update(
    ecs::Entity entityId,
    Direction dir,
    ecs::ComponentManager& compManager)
{
    // On récupère le PositionComponent de ce joueur.
    // getComponent lève une exception si le composant n'existe pas —
    // ce cas ne devrait pas arriver si le joueur est bien initialisé.
    PositionComponent& pos = compManager.getComponent<PositionComponent>(entityId);

    // Calcul de la nouvelle position selon la direction
    int newX = pos.x;
    int newY = pos.y;

    switch (dir) {
        case Direction::UP:    newY -= 1; break;
        case Direction::DOWN:  newY += 1; break;
        case Direction::LEFT:  newX -= 1; break;
        case Direction::RIGHT: newX += 1; break;
    }

    // Vérification des bords de la grille [0, GRID_SIZE - 1]
    if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) {
        // Mouvement bloqué : le joueur reste où il est
        std::cout << "[MovementSystem] Entity " << entityId
                  << " bloquée au bord en (" << pos.x << ", " << pos.y << ")\n";
        return false;
    }

    // Application du mouvement
    pos.x = newX;
    pos.y = newY;

    std::cout << "[MovementSystem] Entity " << entityId
              << " → (" << pos.x << ", " << pos.y << ")\n";
    return true;
}