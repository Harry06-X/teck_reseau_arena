/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** UdpSocket
*/

#include "network/UdpSocket.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace net {

    UdpSocket::UdpSocket() : _fd(socket(AF_INET, SOCK_DGRAM, 0)), _rng(std::random_device{}())
    {
        if (_fd < 0)
            throw SocketError(std::string("socket() failed: ") + std::strerror(errno));
    }

    UdpSocket::~UdpSocket()
    {
        if (_fd != -1)
            close(_fd);
    }

    void UdpSocket::bind(uint16_t port)
    {
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (::bind(_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
            throw SocketError("bind() on port " + std::to_string(port) + " failed: " + std::strerror(errno));
    }

    bool UdpSocket::send(const std::string& data, const sockaddr_in& dest)
    {
        if (shouldDrop())
            return true; // perte simulée : on fait comme si le paquet était parti
        ssize_t sent = sendto(_fd, data.data(), data.size(), 0,
            reinterpret_cast<const sockaddr *>(&dest), sizeof(dest));
        return sent == static_cast<ssize_t>(data.size());
    }

    bool UdpSocket::receive(std::string& out, sockaddr_in& from, int timeoutMs)
    {
        pollfd pfd{_fd, POLLIN, 0};
        if (poll(&pfd, 1, timeoutMs) <= 0 || !(pfd.revents & POLLIN))
            return false;

        char buffer[MAX_DATAGRAM_SIZE];
        socklen_t len = sizeof(from);
        ssize_t bytes = recvfrom(_fd, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&from), &len);
        if (bytes <= 0 || shouldDrop())
            return false;
        out.assign(buffer, static_cast<std::size_t>(bytes));
        return true;
    }

    void UdpSocket::setSimulatedLoss(double ratio)
    {
        _lossRatio = ratio;
    }

    bool UdpSocket::shouldDrop()
    {
        if (_lossRatio <= 0.0)
            return false;
        std::lock_guard<std::mutex> lock(_rngMutex);
        return std::uniform_real_distribution<double>(0.0, 1.0)(_rng) < _lossRatio;
    }

    sockaddr_in resolve(const std::string& host, uint16_t port)
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        addrinfo *result = nullptr;

        if (getaddrinfo(host.c_str(), nullptr, &hints, &result) != 0 || !result)
            throw SocketError("Cannot resolve host '" + host + "'");
        sockaddr_in addr = *reinterpret_cast<sockaddr_in *>(result->ai_addr);
        freeaddrinfo(result);
        addr.sin_port = htons(port);
        return addr;
    }
}
