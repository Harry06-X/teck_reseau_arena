/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** NetworkClient — connexion fiable au serveur, partagée par la CLI et la GUI
*/

#ifndef NETWORKCLIENT_HPP_
#define NETWORKCLIENT_HPP_

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include "network/ProtocolParser.hpp"
#include "network/ReliabilityLayer.hpp"
#include "network/UdpSocket.hpp"

/*
 * Gère tout le protocole côté client : MSGID, ACK, retransmission, doublons.
 * Les messages utiles (hors ACK/doublons) sont transmis au callback, appelé
 * depuis le thread de réception. Un message synthétique "UNREACHABLE" signale
 * qu'un envoi n'a jamais été acquitté.
 */
class NetworkClient {
public:
    using Handler = std::function<void(const net::ParsedMessage&)>;

    NetworkClient(const std::string& host, uint16_t port, double packetLoss, Handler handler);
    ~NetworkClient();

    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;

    // Envoi fiable : MSGID unique + retransmission jusqu'à l'ACK
    void send(const std::string& type, const std::vector<std::string>& fields = {});

    std::string serverAddress() const { return _server.toString(); }

private:
    void receiveLoop();
    void maintenanceLoop();

    net::UdpSocket _socket;
    net::ClientAddress _server;
    net::ReliabilityLayer _reliability;
    Handler _handler;

    std::atomic<bool> _running{true};
    std::thread _receiver;
    std::thread _maintenance;
};

#endif /* !NETWORKCLIENT_HPP_ */
