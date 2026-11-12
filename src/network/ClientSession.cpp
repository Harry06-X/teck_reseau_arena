/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ClientSession
*/

#include "network/ClientSession.hpp"

namespace net {

    ClientSession::ClientSession(const ClientAddress& address, std::string username, ecs::Entity entityId)
        : _address(address), _username(std::move(username)), _entityId(entityId)
    {
        updateLastActivity();
    }

    void ClientSession::updateLastActivity()
    {
        _lastActivity = std::chrono::steady_clock::now();
    }

    bool ClientSession::isTimedOut(std::chrono::seconds timeout) const
    {
        return std::chrono::steady_clock::now() - _lastActivity > timeout;
    }

    ClientSession *SessionManager::find(const ClientAddress& address)
    {
        auto it = _sessions.find(address);
        return it == _sessions.end() ? nullptr : it->second.get();
    }

    ClientSession *SessionManager::findByUsername(const std::string& username)
    {
        for (auto& [address, session] : _sessions)
            if (session->getUsername() == username)
                return session.get();
        return nullptr;
    }

    ClientSession *SessionManager::findByEntity(ecs::Entity entity)
    {
        for (auto& [address, session] : _sessions)
            if (session->getEntityId() == entity)
                return session.get();
        return nullptr;
    }

    ClientSession& SessionManager::open(const ClientAddress& address, const std::string& username, ecs::Entity entity)
    {
        auto& slot = _sessions[address];
        slot = std::make_unique<ClientSession>(address, username, entity);
        return *slot;
    }

    void SessionManager::close(const ClientAddress& address)
    {
        _sessions.erase(address);
    }

    std::vector<ClientSession *> SessionManager::timedOut(std::chrono::seconds timeout)
    {
        std::vector<ClientSession *> expired;
        for (auto& [address, session] : _sessions)
            if (session->isTimedOut(timeout))
                expired.push_back(session.get());
        return expired;
    }

    std::vector<ClientAddress> SessionManager::addresses() const
    {
        std::vector<ClientAddress> result;
        for (const auto& [address, session] : _sessions)
            result.push_back(address);
        return result;
    }
}
