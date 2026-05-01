/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ClientSession
*/

#ifndef CLIENTSESSION_HPP_
#define CLIENTSESSION_HPP_

#include <netinet/in.h>
#include <chrono>
#include "./ecs/Entity.hpp"

namespace net {

    // Structure pour utiliser l'adresse comme clé dans une Map
    struct ClientAddress {
        uint32_t ip;
        uint16_t port;

        // Opérateur nécessaire pour std::map
        bool operator<(const ClientAddress& other) const;
    };

    class ClientSession {
    public:
        explicit ClientSession(ecs::Entity entityId);
        ~ClientSession() = default;

        void updateLastActivity();
        bool isTimedOut(double seconds) const;
        
        ecs::Entity getEntityId() const;

    private:
        ecs::Entity _entityId;
        std::chrono::steady_clock::time_point _lastActivity;
    };
}

#endif /* !CLIENTSESSION_HPP_ */