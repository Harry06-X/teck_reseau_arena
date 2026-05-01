/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** UdpSocket
*/

#include "../include/network/UdpSocket.hpp"
#include <cstring>

namespace net {

    UdpSocket::UdpSocket() : _fd(-1) {
        // Création du socket UDP (SOCK_DGRAM)
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_fd < 0) {
            perror("Socket creation failed");
        }
    }

    UdpSocket::~UdpSocket() {
        if (_fd != -1) {
            close(_fd); // RAII : Fermeture automatique
        }
    }

    bool UdpSocket::bind(int port) {
        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces
        addr.sin_port = htons(port);

        if (::bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Bind failed");
            return false;
        }
        return true;
    }

    ssize_t UdpSocket::sendTo(const std::string& message, const struct sockaddr_in& dest) {
        return sendto(_fd, message.c_str(), message.size(), 0, 
                      (const struct sockaddr *)&dest, sizeof(dest));
    }

    ssize_t UdpSocket::receiveFrom(char* buffer, size_t size, struct sockaddr_in& src) {
        socklen_t len = sizeof(src);
        return recvfrom(_fd, buffer, size, 0, (struct sockaddr *)&src, &len);
    }
}