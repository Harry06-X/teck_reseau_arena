/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Server
*/

#include "network/Server.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include "command/CommandFactory.hpp"
#include "network/ProtocolParser.hpp"

namespace net {

    Server::Server(const ServerConfig& config)
        : _config(config),
          _game(config.seed),
          _accounts(config.accountsFile),
          _ctx{_game, _accounts, _sessions, *this}
    {
        _socket.bind(_config.port);
        _socket.setSimulatedLoss(_config.packetLoss);
    }

    Server::~Server()
    {
        stop();
        if (_networkThread.joinable())
            _networkThread.join();
        if (_maintenanceThread.joinable())
            _maintenanceThread.join();
    }

    void Server::run()
    {
        _running = true;
        _networkThread = std::thread(&Server::networkLoop, this);
        _maintenanceThread = std::thread(&Server::maintenanceLoop, this);

        std::cout << "[Server] Arena Command listening on UDP port " << _config.port
                  << " (session timeout " << _config.sessionTimeout.count() << "s";
        if (_config.packetLoss > 0)
            std::cout << ", simulated loss " << static_cast<int>(_config.packetLoss * 100) << "%";
        std::cout << ")" << std::endl;

        _lastUpdate = Clock::now();
        while (_running) {
            gameTick();
            std::this_thread::sleep_for(TICK);
        }

        _networkThread.join();
        _maintenanceThread.join();
        std::cout << "[Server] Stopped" << std::endl;
    }

    void Server::stop()
    {
        _running = false;
    }

    // ---------------------------------------------------------------- réseau

    void Server::networkLoop()
    {
        std::string raw;
        sockaddr_in from{};
        while (_running) {
            if (_socket.receive(raw, from, 100))
                handleDatagram(raw, ClientAddress::fromSockaddr(from));
        }
    }

    void Server::handleDatagram(const std::string& raw, const ClientAddress& from)
    {
        ParsedMessage msg;
        try {
            msg = ProtocolParser::parse(raw);
        } catch (const ProtocolError& e) {
            reply(from, "ERROR", {e.what()});
            return;
        }

        if (msg.type == "ACK") {
            if (msg.fields.size() == 1 && !msg.fields[0].empty() && msg.fields[0].size() <= 9
                && std::all_of(msg.fields[0].begin(), msg.fields[0].end(), ::isdigit))
                _reliability.acknowledge(from, static_cast<uint32_t>(std::stoul(msg.fields[0])));
            return;
        }
        if (msg.msgId == 0) {
            reply(from, "ERROR", {"Message id must be greater than 0"});
            return;
        }

        // On acquitte toujours, même un doublon : l'ACK précédent a pu se perdre
        sendUnreliable(from, "ACK", {std::to_string(msg.msgId)});
        if (_reliability.isDuplicate(from, msg.msgId)) {
            std::cout << "[Net] Duplicate " << msg.msgId << " from " << from.toString() << " ignored" << std::endl;
            return;
        }

        std::cout << "[Net] <- " << from.toString() << " " << msg.msgId << "|" << msg.type << std::endl;
        try {
            _queue.push(CommandFactory::create(msg, from));
        } catch (const ProtocolError& e) {
            reply(from, "ERROR", {e.what()});
        }
    }

    void Server::maintenanceLoop()
    {
        while (_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::vector<ClientAddress> unreachable;
            for (const auto& packet : _reliability.collectRetransmissions(unreachable)) {
                std::cout << "[Net] Retransmit -> " << packet.dest.toString() << " " << packet.datagram << std::endl;
                _socket.send(packet.datagram, packet.dest.toSockaddr());
            }
            for (const auto& peer : unreachable)
                std::cout << "[Net] " << peer.toString() << " did not ACK after "
                          << ReliabilityLayer::MAX_ATTEMPTS << " attempts, message dropped" << std::endl;
        }
    }

    // ---------------------------------------------------------------- envoi

    void Server::reply(const ClientAddress& to, const std::string& type, const std::vector<std::string>& fields)
    {
        uint32_t id = _reliability.nextMessageId();
        std::string datagram = ProtocolParser::serialize(id, type, fields);
        _reliability.track(id, to, datagram);
        _socket.send(datagram, to.toSockaddr());
    }

    void Server::broadcast(const std::string& type, const std::vector<std::string>& fields)
    {
        for (const auto& address : _sessions.addresses())
            reply(address, type, fields);
    }

    void Server::sendUnreliable(const ClientAddress& to, const std::string& type, const std::vector<std::string>& fields)
    {
        _socket.send(ProtocolParser::serialize(0, type, fields), to.toSockaddr());
    }

    // ---------------------------------------------------------------- jeu

    void Server::gameTick()
    {
        for (auto& command : _queue.popAll())
            command->run(_ctx);

        for (ecs::Entity id : _game.update()) {
            if (ClientSession *session = _sessions.findByEntity(id)) {
                std::cout << "[Game] " << session->getUsername() << " respawned" << std::endl;
                reply(session->getAddress(), "OK", {"You respawned"});
            }
            sendState(id);
        }

        std::vector<ClientAddress> expired;
        for (ClientSession *session : _sessions.timedOut(_config.sessionTimeout))
            expired.push_back(session->getAddress());
        for (const auto& address : expired)
            closeSession(address, "Session timed out after " + std::to_string(_config.sessionTimeout.count())
                + "s of inactivity");

        // GAME_UPDATE non fiable : l'état suivant remplace le précédent,
        // et un keepalive toutes les 2 s rattrape les pertes.
        const auto now = Clock::now();
        if (now - _lastUpdate >= UPDATE_MIN_INTERVAL) {
            bool changed = _game.consumeDirty();
            if (changed || now - _lastUpdate >= UPDATE_KEEPALIVE) {
                const std::string state = _game.serialize();
                for (const auto& address : _sessions.addresses())
                    sendUnreliable(address, "GAME_UPDATE", {state});
                _lastUpdate = now;
            }
        }
    }

    void Server::sendState(ecs::Entity entity)
    {
        ClientSession *session = _sessions.findByEntity(entity);
        if (!session)
            return;
        PlayerState s = _game.getState(entity);
        reply(session->getAddress(), "STATE", {
            std::to_string(s.hp), std::to_string(s.x), std::to_string(s.y), std::to_string(s.score)
        });
    }

    void Server::closeSession(const ClientAddress& address, const std::string& reason)
    {
        ClientSession *session = _sessions.find(address);
        if (!session)
            return;
        std::cout << "[Server] " << session->getUsername() << " disconnected: " << reason << std::endl;
        _game.removePlayer(session->getEntityId());
        _sessions.close(address);
        reply(address, "ERROR", {reason});
    }
}
