/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Address — identifiant d'un pair réseau (IP + port)
*/

#ifndef ADDRESS_HPP_
#define ADDRESS_HPP_

#include <netinet/in.h>
#include <cstdint>
#include <string>

namespace net {

    // Clé unique d'un client : une session = un couple IP/port
    struct ClientAddress {
        uint32_t ip = 0;    // ordre réseau
        uint16_t port = 0;  // ordre réseau

        static ClientAddress fromSockaddr(const sockaddr_in& addr);
        sockaddr_in toSockaddr() const;
        std::string toString() const;

        bool operator<(const ClientAddress& other) const;
        bool operator==(const ClientAddress& other) const;
    };
}

#endif /* !ADDRESS_HPP_ */
