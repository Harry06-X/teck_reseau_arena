/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ReliabilityLayer — fiabilité applicative au-dessus d'UDP
**   - identifiant unique par message (compteur atomique)
**   - ACK|MSGID à chaque réception
**   - timeout + retransmission (3 s, 5 envois max)
**   - détection des doublons par pair
*/

#ifndef RELIABILITYLAYER_HPP_
#define RELIABILITYLAYER_HPP_

#include <atomic>
#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>
#include "network/Address.hpp"

namespace net {

    class ReliabilityLayer {
    public:
        using Clock = std::chrono::steady_clock;

        static constexpr std::chrono::milliseconds RETRY_DELAY{3000};
        static constexpr int MAX_ATTEMPTS = 5;              // envoi initial compris
        static constexpr std::size_t MAX_REMEMBERED_IDS = 1024; // par pair

        struct Outgoing {
            ClientAddress dest;
            std::string datagram;
        };

        explicit ReliabilityLayer(uint32_t firstId = 1);

        // Nouvel identifiant unique (thread-safe, jamais 0)
        uint32_t nextMessageId();

        // Mémorise un message envoyé qui attend son ACK
        void track(uint32_t msgId, const ClientAddress& dest, const std::string& datagram);

        // ACK reçu : le message n'est plus à retransmettre
        void acknowledge(const ClientAddress& from, uint32_t msgId);

        // Enregistre l'ID reçu ; retourne true s'il a déjà été vu (doublon)
        bool isDuplicate(const ClientAddress& from, uint32_t msgId);

        // Messages dont le délai est écoulé : à renvoyer.
        // Les pairs abandonnés après MAX_ATTEMPTS sont ajoutés à `unreachable`.
        std::vector<Outgoing> collectRetransmissions(std::vector<ClientAddress>& unreachable);

        // Oublie tout ce qui concerne un pair (fin de session)
        void forget(const ClientAddress& peer);

        std::size_t pendingCount() const;

    private:
        struct PendingPacket {
            ClientAddress dest;
            std::string datagram;
            Clock::time_point lastSent;
            int attempts;
        };

        struct PeerHistory {
            std::unordered_set<uint32_t> ids;
            std::deque<uint32_t> order; // pour borner la mémoire (FIFO)
        };

        std::atomic<uint32_t> _nextId;
        std::map<uint32_t, PendingPacket> _pending;
        std::map<ClientAddress, PeerHistory> _received;
        mutable std::mutex _mutex;
    };
}

#endif /* !RELIABILITYLAYER_HPP_ */
