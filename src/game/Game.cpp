/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Game
*/

#include "game/Game.hpp"
#include <set>
#include <stdexcept>
#include "entities/Archer.hpp"
#include "entities/Warrior.hpp"

Game::Game(unsigned int seed) : _rng(seed)
{
    _components.registerComponent<HealthComponent>();
    _components.registerComponent<PositionComponent>();
    _components.registerComponent<ScoreComponent>();
}

ecs::Entity Game::spawnPlayer(const std::string& username, PlayerClass playerClass)
{
    ecs::Entity id = _entities.createEntity();
    std::unique_ptr<Player> player;
    if (playerClass == PlayerClass::Archer)
        player = std::make_unique<Archer>(id, username);
    else
        player = std::make_unique<Warrior>(id, username);

    PositionComponent cell = randomFreeCell();
    player->initialize(_components, cell.x, cell.y);
    _players[id] = std::move(player);
    _dirty = true;
    return id;
}

void Game::removePlayer(ecs::Entity id)
{
    if (!hasPlayer(id))
        return;
    _players.erase(id);
    _respawn.forget(id);
    _components.entityDestroyed(id);
    _entities.destroyEntity(id);
    _dirty = true;
}

bool Game::hasPlayer(ecs::Entity id) const
{
    return _players.count(id) > 0;
}

bool Game::isAlive(ecs::Entity id)
{
    return hasPlayer(id) && _components.getComponent<HealthComponent>(id).hp > 0;
}

int Game::respawnIn(ecs::Entity id) const
{
    return _respawn.secondsLeft(id);
}

const Player& Game::getPlayer(ecs::Entity id) const
{
    auto it = _players.find(id);
    if (it == _players.end())
        throw std::out_of_range("Unknown player " + std::to_string(id));
    return *it->second;
}

bool Game::move(ecs::Entity id, MovementSystem::Direction dir)
{
    if (!isAlive(id))
        return false;
    bool moved = _movement.update(id, dir, _components);
    _dirty = _dirty || moved;
    return moved;
}

CombatSystem::Outcome Game::attack(ecs::Entity attacker, ecs::Entity target)
{
    auto outcome = _combat.attack(getPlayer(attacker), getPlayer(target), _components);
    if (outcome == CombatSystem::Outcome::Killed)
        _respawn.handleDeath(target);
    if (outcome == CombatSystem::Outcome::Hit || outcome == CombatSystem::Outcome::Killed)
        _dirty = true;
    return outcome;
}

std::vector<ecs::Entity> Game::update()
{
    std::vector<ecs::Entity> respawned;
    for (ecs::Entity id : _respawn.update()) {
        if (!hasPlayer(id))
            continue;
        _components.getComponent<HealthComponent>(id).hp = Player::STARTING_HP;
        _components.getComponent<PositionComponent>(id) = randomFreeCell();
        respawned.push_back(id);
        _dirty = true;
    }
    return respawned;
}

PlayerState Game::getState(ecs::Entity id)
{
    const auto& pos = _components.getComponent<PositionComponent>(id);
    return {
        _components.getComponent<HealthComponent>(id).hp,
        pos.x,
        pos.y,
        _components.getComponent<ScoreComponent>(id).score
    };
}

std::string Game::serialize()
{
    std::string out;
    for (const auto& [id, player] : _players) {
        PlayerState s = getState(id);
        if (!out.empty())
            out += ';';
        out += std::to_string(id) + ',' + player->getUsername() + ',' + player->getClassName() + ','
            + std::to_string(s.hp) + ',' + std::to_string(s.x) + ',' + std::to_string(s.y) + ','
            + std::to_string(s.score);
    }
    return out;
}

bool Game::consumeDirty()
{
    bool wasDirty = _dirty;
    _dirty = false;
    return wasDirty;
}

PositionComponent Game::randomFreeCell()
{
    std::set<std::pair<int, int>> taken;
    for (const auto& [id, player] : _players) {
        try {
            const auto& pos = _components.getComponent<PositionComponent>(id);
            taken.insert({pos.x, pos.y});
        } catch (const std::runtime_error&) {
            // joueur en cours de création : pas encore de position
        }
    }

    std::uniform_int_distribution<int> dist(0, MovementSystem::GRID_SIZE - 1);
    for (int attempt = 0; attempt < 100; ++attempt) {
        PositionComponent cell{dist(_rng), dist(_rng)};
        if (!taken.count({cell.x, cell.y}))
            return cell;
    }
    return {dist(_rng), dist(_rng)}; // arène saturée : on accepte une case partagée
}
