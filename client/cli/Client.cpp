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
#include <sstream>
#include <thread>
#include "ArenaView.hpp"

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
}

Client::Client(const std::string& host, uint16_t port, double packetLoss)
    : _interactive(isatty(STDOUT_FILENO)),
      _state(!_interactive),
      _network(std::make_unique<NetworkClient>(host, port, packetLoss,
          [this](const net::ParsedMessage& msg) { onMessage(msg); }))
{
    print("Connected to " + _network->serverAddress() + " — type 'help' to see the commands");
}

void Client::run()
{
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!handleInput(line))
            break;
    }

    // Déconnexion propre : on attend la réponse au LOGOUT (4 s max, une retransmission)
    bool loggedIn = false;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        loggedIn = _state.loggedIn();
    }
    if (loggedIn) {
        _network->send("LOGOUT");
        for (int i = 0; i < 80 && loggedIn; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::lock_guard<std::mutex> lock(_mutex);
            loggedIn = _state.loggedIn();
        }
    }
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
        if (cmd == "login") {
            std::lock_guard<std::mutex> lock(_mutex);
            _state.setPendingLogin(args[1]);
        }
        _network->send(toUpper(cmd), fields);
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
            _network->send("MOVE", {dir});
            return true;
        }
    }

    if (cmd == "attack" && args.size() == 2)
        _network->send("ATTACK", {args[1]});
    else if (cmd == "status" || cmd == "look" || cmd == "logout")
        _network->send(toUpper(cmd));
    else
        print("Unknown command '" + line + "' — type 'help'");
    return true;
}

void Client::showHelp()
{
    print("register <user> <pass>              create an account");
    print("login <user> <pass> [warrior|archer] join the arena (warrior range 1, archer range 3)");
    print("w/a/s/d (or z/q/s/d, up/down...)    move one cell");
    print("attack <id>                         attack a player (20 damage)");
    print("status · look · logout · quit");
}

// ---------------------------------------------------------------- affichage

void Client::print(const std::string& line)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!line.empty()) {
        _state.addLog(line);
        if (!_interactive)
            std::cout << line << std::endl;
    }
    if (_interactive)
        ArenaView::render(_state);
}

void Client::onMessage(const net::ParsedMessage& msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto line = _state.apply(msg);
    if (_interactive)
        ArenaView::render(_state);
    else if (line)
        std::cout << *line << std::endl;
}
