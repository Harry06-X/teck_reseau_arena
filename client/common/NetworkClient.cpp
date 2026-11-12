/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** NetworkClient
*/

#include "NetworkClient.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>

namespace {
    // ID de départ aléatoire : un client relancé sur le même port ne
    // sera pas pris pour un doublon par le serveur
    uint32_t randomFirstId()
    {
        std::mt19937 rng(std::random_device{}());
        return std::uniform_int_distribution<uint32_t>(1, 100000000)(rng);
    }
}

NetworkClient::NetworkClient(const std::string& host, uint16_t port, double packetLoss, Handler handler)
    : _server(net::ClientAddress::fromSockaddr(net::resolve(host, port))),
      _reliability(randomFirstId()),
      _handler(std::move(handler))
{
    _socket.setSimulatedLoss(packetLoss);
    _receiver = std::thread(&NetworkClient::receiveLoop, this);
    _maintenance = std::thread(&NetworkClient::maintenanceLoop, this);
}

NetworkClient::~NetworkClient()
{
    _running = false;
    if (_receiver.joinable())
        _receiver.join();
    if (_maintenance.joinable())
        _maintenance.join();
}

void NetworkClient::send(const std::string& type, const std::vector<std::string>& fields)
{
    uint32_t id = _reliability.nextMessageId();
    std::string datagram = net::ProtocolParser::serialize(id, type, fields);
    _reliability.track(id, _server, datagram);
    _socket.send(datagram, _server.toSockaddr());
}

void NetworkClient::receiveLoop()
{
    std::string raw;
    sockaddr_in from{};
    while (_running) {
        if (!_socket.receive(raw, from, 100))
            continue;
        if (!(net::ClientAddress::fromSockaddr(from) == _server))
            continue; // on n'écoute que le serveur

        net::ParsedMessage msg;
        try {
            msg = net::ProtocolParser::parse(raw);
        } catch (const net::ProtocolError& e) {
            _handler({0, "ERROR", {std::string("Invalid message from server: ") + e.what()}});
            continue;
        }

        if (msg.type == "ACK") {
            if (msg.fields.size() == 1 && !msg.fields[0].empty() && msg.fields[0].size() <= 9
                && std::all_of(msg.fields[0].begin(), msg.fields[0].end(), ::isdigit))
                _reliability.acknowledge(_server, static_cast<uint32_t>(std::stoul(msg.fields[0])));
            continue;
        }
        if (msg.msgId != 0) {
            _socket.send(net::ProtocolParser::serialize(0, "ACK", {std::to_string(msg.msgId)}), _server.toSockaddr());
            if (_reliability.isDuplicate(_server, msg.msgId))
                continue;
        }
        _handler(msg);
    }
}

void NetworkClient::maintenanceLoop()
{
    while (_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::vector<net::ClientAddress> unreachable;
        for (const auto& packet : _reliability.collectRetransmissions(unreachable))
            _socket.send(packet.datagram, packet.dest.toSockaddr());
        if (!unreachable.empty())
            _handler({0, "UNREACHABLE", {"Server did not answer after "
                + std::to_string(net::ReliabilityLayer::MAX_ATTEMPTS) + " attempts, is it running?"}});
    }
}
