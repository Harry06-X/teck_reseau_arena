/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ClientSession
*/

#include "../include/network/ClientSession.hpp"
#include <tuple>

namespace net {

    bool ClientAddress::operator<(const ClientAddress& other) const {
        // On compare l'IP puis le Port pour créer un ordre unique
        return std::tie(ip, port) < std::tie(other.ip, other.port);
    }

    ClientSession::ClientSession(ecs::Entity entityId) 
        : _entityId(entityId) {
        updateLastActivity();
    }

    void ClientSession::updateLastActivity() {
        _lastActivity = std::chrono::steady_clock::now();
    }

    bool ClientSession::isTimedOut(double seconds) const {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - _lastActivity;
        return elapsed.count() > seconds;
    }

    ecs::Entity ClientSession::getEntityId() const {
        return _entityId;
    }
}