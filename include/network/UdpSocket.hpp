/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** UdpSocket
*/

#ifndef UDPSOCKET_HPP_
#define UDPSOCKET_HPP_

#include <netinet/in.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

namespace net {
    class UdpSocket {
    public:
        UdpSocket();
        ~UdpSocket();

        // Ouvre le socket et le lie à un port
        bool bind(int port);
        
        // Envoi/Réception basique
        ssize_t sendTo(const std::string& message, const struct sockaddr_in& dest);
        ssize_t receiveFrom(char* buffer, size_t size, struct sockaddr_in& src);

        int getFd() const { return _fd; }

    private:
        int _fd; // Le descripteur de fichier du socket
    };
}

#endif /* !UDPSOCKET_HPP_ */
