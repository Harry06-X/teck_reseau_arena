/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Game — état autoritaire de l'arène (ECS + systèmes)
*/

#ifndef GAME_HPP_
#define GAME_HPP_

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "ecs/ComponentManager.hpp"
#include "ecs/EntityManager.hpp"
#include "entities/Player.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/RespawnSystem.hpp"

struct PlayerState {
    int hp;
    int x;
    int y;
    int score;
};

class Game {
public:
    enum class PlayerClass { Warrior, Archer };

    explicit Game(unsigned int seed = std::random_device{}());

    // Crée l'entité ECS + l'objet Player (Warrior/Archer) sur une case libre
    ecs::Entity spawnPlayer(const std::string& username, PlayerClass playerClass);
    void removePlayer(ecs::Entity id);

    bool hasPlayer(ecs::Entity id) const;
    bool isAlive(ecs::Entity id);
    int respawnIn(ecs::Entity id) const;
    const Player& getPlayer(ecs::Entity id) const;

    // Règles de jeu (déléguées aux systèmes)
    bool move(ecs::Entity id, MovementSystem::Direction dir);
    CombatSystem::Outcome attack(ecs::Entity attacker, ecs::Entity target);

    // À appeler à chaque tick : retourne les joueurs réapparus
    std::vector<ecs::Entity> update();

    PlayerState getState(ecs::Entity id);

    // GAME_UPDATE : "id,nom,classe,hp,x,y,score;id,nom,..."
    std::string serialize();

    // true si l'état a changé depuis le dernier appel
    bool consumeDirty();

private:
    PositionComponent randomFreeCell();

    ecs::EntityManager _entities;
    ecs::ComponentManager _components;
    MovementSystem _movement;
    CombatSystem _combat;
    RespawnSystem _respawn;
    std::map<ecs::Entity, std::unique_ptr<Player>> _players;
    std::mt19937 _rng;
    bool _dirty = false;
};

#endif /* !GAME_HPP_ */
