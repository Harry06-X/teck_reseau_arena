/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Address
*/

#include "network/Address.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <tuple>

namespace net {

    ClientAddress ClientAddress::fromSockaddr(const sockaddr_in& addr)
    {
        return { addr.sin_addr.s_addr, addr.sin_port };
    }

    sockaddr_in ClientAddress::toSockaddr() const
    {
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ip;
        addr.sin_port = port;
        return addr;
    }

    std::string ClientAddress::toString() const
    {
        char buffer[INET_ADDRSTRLEN] = {0};
        in_addr raw{};
        raw.s_addr = ip;
        inet_ntop(AF_INET, &raw, buffer, sizeof(buffer));
        return std::string(buffer) + ":" + std::to_string(ntohs(port));
    }

    bool ClientAddress::operator<(const ClientAddress& other) const
    {
        // On compare l'IP puis le port pour créer un ordre unique
        return std::tie(ip, port) < std::tie(other.ip, other.port);
    }

    bool ClientAddress::operator==(const ClientAddress& other) const
    {
        return ip == other.ip && port == other.port;
    }
}
