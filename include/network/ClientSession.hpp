/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ClientSession — un joueur connecté (IP + port ↔ compte ↔ entité ECS)
*/

#ifndef CLIENTSESSION_HPP_
#define CLIENTSESSION_HPP_

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ecs/Entity.hpp"
#include "network/Address.hpp"

namespace net {

    class ClientSession {
    public:
        static constexpr std::chrono::seconds DEFAULT_TIMEOUT{60};

        ClientSession(const ClientAddress& address, std::string username, ecs::Entity entityId);
        ~ClientSession() = default;

        void updateLastActivity();
        bool isTimedOut(std::chrono::seconds timeout = DEFAULT_TIMEOUT) const;

        const ClientAddress& getAddress() const { return _address; }
        const std::string& getUsername() const { return _username; }
        ecs::Entity getEntityId() const { return _entityId; }

    private:
        ClientAddress _address;
        std::string _username;
        ecs::Entity _entityId;
        std::chrono::steady_clock::time_point _lastActivity;
    };

    // Registre des sessions — utilisé uniquement par le thread de jeu
    class SessionManager {
    public:
        ClientSession *find(const ClientAddress& address);
        ClientSession *findByUsername(const std::string& username);
        ClientSession *findByEntity(ecs::Entity entity);

        ClientSession& open(const ClientAddress& address, const std::string& username, ecs::Entity entity);
        void close(const ClientAddress& address);

        std::vector<ClientSession *> timedOut(std::chrono::seconds timeout);
        std::vector<ClientAddress> addresses() const;

    private:
        std::map<ClientAddress, std::unique_ptr<ClientSession>> _sessions;
    };
}

#endif /* !CLIENTSESSION_HPP_ */
