/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ReliabilityLayer
*/

#include "network/ReliabilityLayer.hpp"

namespace net {

    ReliabilityLayer::ReliabilityLayer(uint32_t firstId) : _nextId(firstId == 0 ? 1 : firstId) {}

    uint32_t ReliabilityLayer::nextMessageId()
    {
        uint32_t id = _nextId.fetch_add(1);
        // 0 est réservé aux messages sans ACK ; le format limite l'ID à 9 chiffres
        if (id == 0 || id > 999999999) {
            _nextId = 2;
            id = 1;
        }
        return id;
    }

    void ReliabilityLayer::track(uint32_t msgId, const ClientAddress& dest, const std::string& datagram)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _pending[msgId] = PendingPacket{dest, datagram, Clock::now(), 1};
    }

    void ReliabilityLayer::acknowledge(const ClientAddress& from, uint32_t msgId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _pending.find(msgId);
        // On vérifie l'émetteur : un pair ne peut pas acquitter le message d'un autre
        if (it != _pending.end() && it->second.dest == from)
            _pending.erase(it);
    }

    bool ReliabilityLayer::isDuplicate(const ClientAddress& from, uint32_t msgId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        PeerHistory& history = _received[from];
        if (history.ids.count(msgId))
            return true;
        history.ids.insert(msgId);
        history.order.push_back(msgId);
        if (history.order.size() > MAX_REMEMBERED_IDS) {
            history.ids.erase(history.order.front());
            history.order.pop_front();
        }
        return false;
    }

    std::vector<ReliabilityLayer::Outgoing> ReliabilityLayer::collectRetransmissions(
        std::vector<ClientAddress>& unreachable)
    {
        std::vector<Outgoing> toSend;
        const auto now = Clock::now();
        std::lock_guard<std::mutex> lock(_mutex);

        for (auto it = _pending.begin(); it != _pending.end(); ) {
            PendingPacket& packet = it->second;
            if (now - packet.lastSent < RETRY_DELAY) {
                ++it;
                continue;
            }
            if (packet.attempts >= MAX_ATTEMPTS) {
                unreachable.push_back(packet.dest);
                it = _pending.erase(it);
                continue;
            }
            packet.attempts++;
            packet.lastSent = now;
            toSend.push_back({packet.dest, packet.datagram});
            ++it;
        }
        return toSend;
    }

    void ReliabilityLayer::forget(const ClientAddress& peer)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _received.erase(peer);
        for (auto it = _pending.begin(); it != _pending.end(); ) {
            if (it->second.dest == peer)
                it = _pending.erase(it);
            else
                ++it;
        }
    }

    std::size_t ReliabilityLayer::pendingCount() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _pending.size();
    }
}
