/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ClientState
*/

#include "ClientState.hpp"
#include <algorithm>
#include <regex>
#include <sstream>

std::optional<std::string> ClientState::apply(const net::ParsedMessage& msg)
{
    const std::string text = msg.fields.empty() ? "" : msg.fields[0];
    std::optional<std::string> line;

    if (msg.type == "OK") {
        if (text.rfind("Welcome", 0) == 0) {
            _loggedIn = true;
            _username = _pendingUsername;
            std::smatch match;
            if (std::regex_search(text, match, std::regex("player (\\d+)")))
                _selfId = std::stoul(match[1]);
        } else if (text.rfind("Goodbye", 0) == 0) {
            reset();
        } else if (text == "You respawned") {
            _diedAt.reset();
        }
        line = "✔ " + text;
    } else if (msg.type == "ERROR" || msg.type == "UNREACHABLE") {
        if (text.find("timed out") != std::string::npos)
            reset();
        line = "✖ " + text;
    } else if (msg.type == "STATE" && msg.fields.size() == 4) {
        try {
            _hp = std::stoi(msg.fields[0]);
            _x = std::stoi(msg.fields[1]);
            _y = std::stoi(msg.fields[2]);
            _score = std::stoi(msg.fields[3]);
            _hasState = true;
            if (_hp > 0)
                _diedAt.reset();
            if (_verbose)
                line = "STATE hp=" + msg.fields[0] + " pos=(" + msg.fields[1] + "," + msg.fields[2]
                    + ") score=" + msg.fields[3];
        } catch (const std::exception&) {
            line = "✖ Invalid STATE";
        }
    } else if (msg.type == "PLAYER_KILLED" && msg.fields.size() == 1) {
        try {
            unsigned long id = std::stoul(text);
            if (_selfId && *_selfId == id) {
                _diedAt = Clock::now();
                line = "☠ You were eliminated, respawn in " + std::to_string(RESPAWN_SECONDS) + "s";
            } else {
                line = "☠ " + nameOf(id) + " was eliminated";
            }
        } catch (const std::exception&) {
            line = "✖ Invalid PLAYER_KILLED";
        }
    } else if (msg.type == "GAME_UPDATE") {
        _players = parseGameUpdate(text);
        // Un LOOK explicite (fiable, MSGID > 0) mérite une trace en mode non interactif
        if (_verbose && msg.msgId != 0)
            line = "GAME_UPDATE " + text;
    } else {
        line = "[" + msg.type + "] " + text;
    }

    if (line)
        addLog(*line);
    return line;
}

void ClientState::addLog(const std::string& line)
{
    _log.push_back(line);
    while (_log.size() > MAX_LOG)
        _log.pop_front();
}

int ClientState::respawnIn() const
{
    if (!_diedAt)
        return 0;
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - *_diedAt).count();
    return std::max(0, RESPAWN_SECONDS - static_cast<int>(elapsed));
}

const RemotePlayer *ClientState::self() const
{
    for (const auto& p : _players)
        if ((_selfId && p.id == *_selfId) || (!_selfId && p.name == _username))
            return &p;
    return nullptr;
}

const RemotePlayer *ClientState::playerAt(int x, int y) const
{
    const RemotePlayer *found = nullptr;
    for (const auto& p : _players)
        if (p.x == x && p.y == y && (!found || !found->alive()))
            found = &p; // un joueur vivant est prioritaire sur un mort
    return found;
}

std::string ClientState::nameOf(unsigned long id) const
{
    for (const auto& p : _players)
        if (p.id == id)
            return p.name + " (" + std::to_string(id) + ")";
    return "player " + std::to_string(id);
}

void ClientState::reset()
{
    _loggedIn = false;
    _username.clear();
    _selfId.reset();
    _hasState = false;
    _diedAt.reset();
    _players.clear();
}

std::vector<RemotePlayer> ClientState::parseGameUpdate(const std::string& payload)
{
    std::vector<RemotePlayer> players;
    std::istringstream entries(payload);
    std::string entry;

    while (std::getline(entries, entry, ';')) {
        std::istringstream fields(entry);
        std::vector<std::string> f;
        std::string field;
        while (std::getline(fields, field, ','))
            f.push_back(field);
        if (f.size() != 7)
            continue;
        try {
            RemotePlayer p;
            p.id = std::stoul(f[0]);
            p.name = f[1];
            p.playerClass = f[2];
            p.hp = std::stoi(f[3]);
            p.x = std::stoi(f[4]);
            p.y = std::stoi(f[5]);
            p.score = std::stoi(f[6]);
            players.push_back(p);
        } catch (const std::exception&) {
            // entrée corrompue : ignorée
        }
    }
    return players;
}
