/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Client
*/

#include "Client.hpp"
#include <unistd.h>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>

namespace {
    std::string toUpper(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
        return s;
    }

    std::string toLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    // ID de départ aléatoire : un client relancé sur le même port ne
    // sera pas pris pour un doublon par le serveur
    uint32_t randomFirstId()
    {
        std::mt19937 rng(std::random_device{}());
        return std::uniform_int_distribution<uint32_t>(1, 100000000)(rng);
    }
}

Client::Client(const std::string& host, uint16_t port, double packetLoss)
    : _server(net::ClientAddress::fromSockaddr(net::resolve(host, port))),
      _reliability(randomFirstId()),
      _view(isatty(STDOUT_FILENO))
{
    _socket.setSimulatedLoss(packetLoss);
    _receiver = std::thread(&Client::receiveLoop, this);
    _maintenance = std::thread(&Client::maintenanceLoop, this);
    print("Connected to " + _server.toString() + " — type 'help' to see the commands");
}

Client::~Client()
{
    _running = false;
    if (_receiver.joinable())
        _receiver.join();
    if (_maintenance.joinable())
        _maintenance.join();
}

void Client::run()
{
    std::string line;
    while (_running && std::getline(std::cin, line)) {
        if (!handleInput(line))
            break;
    }

    // Déconnexion propre : on attend la réponse au LOGOUT (4 s max, une retransmission)
    if (_loggedIn) {
        sendCommand("LOGOUT");
        for (int i = 0; i < 80 && _loggedIn; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    _running = false;
}

// ---------------------------------------------------------------- clavier

bool Client::handleInput(const std::string& line)
{
    std::istringstream in(line);
    std::vector<std::string> args;
    std::string word;
    while (in >> word)
        args.push_back(word);
    if (args.empty()) {
        print("");
        return true;
    }

    const std::string cmd = toLower(args[0]);
    if (cmd == "quit" || cmd == "exit")
        return false;
    if (cmd == "help") {
        showHelp();
        return true;
    }

    if (cmd == "register" || cmd == "login") {
        if (args.size() < 3 || (cmd == "register" && args.size() != 3) || args.size() > 4) {
            print(cmd == "login" ? "Usage: login <username> <password> [warrior|archer]"
                                 : "Usage: register <username> <password>");
            return true;
        }
        if (!net::ProtocolParser::isValidUsername(args[1]) || !net::ProtocolParser::isValidPassword(args[2])) {
            print("Username: 3-16 chars [A-Za-z0-9_] — password: 4-32 chars, no '|'");
            return true;
        }
        std::vector<std::string> fields{args[1], args[2]};
        if (args.size() == 4)
            fields.push_back(toUpper(args[3]));
        if (cmd == "login")
            _pendingUsername = args[1];
        sendCommand(toUpper(cmd), fields);
        return true;
    }

    // Raccourcis de déplacement QWERTY (wasd) et AZERTY (zqsd)
    static const std::vector<std::pair<std::string, std::string>> moves = {
        {"w", "UP"}, {"z", "UP"}, {"up", "UP"}, {"s", "DOWN"}, {"down", "DOWN"},
        {"a", "LEFT"}, {"q", "LEFT"}, {"left", "LEFT"}, {"d", "RIGHT"}, {"right", "RIGHT"}
    };
    std::string direction = cmd == "move" && args.size() == 2 ? toLower(args[1]) : cmd;
    for (const auto& [key, dir] : moves) {
        if (direction == key) {
            sendCommand("MOVE", {dir});
            return true;
        }
    }

    if (cmd == "attack" && args.size() == 2)
        sendCommand("ATTACK", {args[1]});
    else if (cmd == "status" || cmd == "look" || cmd == "logout")
        sendCommand(toUpper(cmd));
    else
        print("Unknown command '" + line + "' — type 'help'");
    return true;
}

void Client::sendCommand(const std::string& type, const std::vector<std::string>& fields)
{
    uint32_t id = _reliability.nextMessageId();
    std::string datagram = net::ProtocolParser::serialize(id, type, fields);
    _reliability.track(id, _server, datagram);
    _socket.send(datagram, _server.toSockaddr());
}

void Client::showHelp()
{
    print("register <user> <pass>              create an account");
    print("login <user> <pass> [warrior|archer] join the arena (warrior range 1, archer range 3)");
    print("w/a/s/d (or z/q/s/d, up/down...)    move one cell");
    print("attack <id>                         attack a player (20 damage)");
    print("status · look · logout · quit");
}

void Client::print(const std::string& line)
{
    std::lock_guard<std::mutex> lock(_viewMutex);
    if (!line.empty())
        _view.log(line);
    _view.render();
}

// ---------------------------------------------------------------- réseau

void Client::receiveLoop()
{
    std::string raw;
    sockaddr_in from{};
    while (_running) {
        if (!_socket.receive(raw, from, 100))
            continue;
        if (!(net::ClientAddress::fromSockaddr(from) == _server))
            continue; // on n'écoute que le serveur
        try {
            handleMessage(net::ProtocolParser::parse(raw));
        } catch (const net::ProtocolError& e) {
            print(std::string("[invalid message from server: ") + e.what() + "]");
        }
    }
}

void Client::maintenanceLoop()
{
    while (_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::vector<net::ClientAddress> unreachable;
        for (const auto& packet : _reliability.collectRetransmissions(unreachable))
            _socket.send(packet.datagram, packet.dest.toSockaddr());
        if (!unreachable.empty())
            print("✖ Server did not answer after " + std::to_string(net::ReliabilityLayer::MAX_ATTEMPTS)
                + " attempts — is it running?");
    }
}

void Client::handleMessage(const net::ParsedMessage& msg)
{
    if (msg.type == "ACK") {
        if (msg.fields.size() == 1 && !msg.fields[0].empty()
            && std::all_of(msg.fields[0].begin(), msg.fields[0].end(), ::isdigit))
            _reliability.acknowledge(_server, static_cast<uint32_t>(std::stoul(msg.fields[0])));
        return;
    }
    if (msg.msgId != 0) {
        _socket.send(net::ProtocolParser::serialize(0, "ACK", {std::to_string(msg.msgId)}), _server.toSockaddr());
        if (_reliability.isDuplicate(_server, msg.msgId))
            return;
    }

    const std::string text = msg.fields.empty() ? "" : msg.fields[0];
    std::lock_guard<std::mutex> lock(_viewMutex);

    if (msg.type == "OK") {
        if (text.rfind("Welcome", 0) == 0) {
            _loggedIn = true;
            _view.setUsername(_pendingUsername);
        } else if (text.rfind("Goodbye", 0) == 0) {
            _loggedIn = false;
            _view.setUsername("");
            _view.setPlayers({});
        }
        _view.log("✔ " + text);
    } else if (msg.type == "ERROR") {
        if (text.find("timed out") != std::string::npos) {
            _loggedIn = false;
            _view.setUsername("");
            _view.setPlayers({});
        }
        _view.log("✖ " + text);
    } else if (msg.type == "STATE" && msg.fields.size() == 4) {
        try {
            _view.setSelfState(std::stoi(msg.fields[0]), std::stoi(msg.fields[1]),
                std::stoi(msg.fields[2]), std::stoi(msg.fields[3]));
            if (!isatty(STDOUT_FILENO))
                _view.log("STATE hp=" + msg.fields[0] + " pos=(" + msg.fields[1] + "," + msg.fields[2]
                    + ") score=" + msg.fields[3]);
        } catch (const std::exception&) {
            _view.log("[invalid STATE]");
        }
    } else if (msg.type == "PLAYER_KILLED" && msg.fields.size() == 1) {
        try {
            _view.log("☠ " + _view.nameOf(std::stoul(text)) + " was eliminated");
        } catch (const std::exception&) {
            _view.log("[invalid PLAYER_KILLED]");
        }
    } else if (msg.type == "GAME_UPDATE") {
        _view.setPlayers(ArenaView::parseGameUpdate(text));
        // Un LOOK explicite (fiable) mérite une trace en mode non interactif
        if (msg.msgId != 0)
            _view.log("GAME_UPDATE " + text);
    } else {
        _view.log("[" + msg.type + "] " + text);
    }
    _view.render();
}
