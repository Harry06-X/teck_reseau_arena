/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** UdpSocket — wrapper RAII autour d'un socket UDP
*/

#ifndef UDPSOCKET_HPP_
#define UDPSOCKET_HPP_

#include <netinet/in.h>
#include <cstdint>
#include <mutex>
#include <random>
#include <stdexcept>
#include <string>

namespace net {

    class SocketError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class UdpSocket {
    public:
        static constexpr std::size_t MAX_DATAGRAM_SIZE = 4096;

        UdpSocket();   // lève SocketError
        ~UdpSocket();  // RAII : ferme le descripteur

        UdpSocket(const UdpSocket&) = delete;
        UdpSocket& operator=(const UdpSocket&) = delete;

        // Lie le socket à un port local (côté serveur)
        void bind(uint16_t port);

        // sendto() est thread-safe : plusieurs threads peuvent envoyer en parallèle
        bool send(const std::string& data, const sockaddr_in& dest);

        // Attend au plus timeoutMs un datagramme (poll). Retourne false si rien reçu.
        bool receive(std::string& out, sockaddr_in& from, int timeoutMs);

        // Simule une perte de paquets (0.0 → 1.0) pour tester la fiabilité
        void setSimulatedLoss(double ratio);

    private:
        bool shouldDrop();

        int _fd;
        double _lossRatio = 0.0;
        std::mt19937 _rng;
        std::mutex _rngMutex;
    };

    // Résout "host" (IP ou nom) en adresse IPv4 — lève SocketError
    sockaddr_in resolve(const std::string& host, uint16_t port);
}

#endif /* !UDPSOCKET_HPP_ */
