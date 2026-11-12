/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ArenaView
*/

#include "ArenaView.hpp"
#include <iostream>
#include <sstream>

namespace {
    const char *RESET = "\033[0m";
    const char *BOLD = "\033[1m";
    const char *GREEN = "\033[32m";
    const char *RED = "\033[31m";
    const char *DIM = "\033[2m";
}

ArenaView::ArenaView(bool interactive) : _interactive(interactive) {}

void ArenaView::setUsername(const std::string& username)
{
    _username = username;
    if (username.empty())
        _hasState = false;
}

void ArenaView::setSelfState(int hp, int x, int y, int score)
{
    _hasState = true;
    _hp = hp;
    _x = x;
    _y = y;
    _score = score;
}

void ArenaView::setPlayers(std::vector<RemotePlayer> players)
{
    _players = std::move(players);
}

void ArenaView::log(const std::string& line)
{
    if (!_interactive) {
        std::cout << line << std::endl;
        return;
    }
    _log.push_back(line);
    while (_log.size() > LOG_LINES)
        _log.pop_front();
}

std::string ArenaView::nameOf(unsigned long id) const
{
    for (const auto& p : _players)
        if (p.id == id)
            return p.name + " (" + std::to_string(id) + ")";
    return "player " + std::to_string(id);
}

char ArenaView::symbolFor(const RemotePlayer& player)
{
    if (player.hp <= 0)
        return 'x';
    if (player.id < 10)
        return static_cast<char>('0' + player.id);
    return static_cast<char>('A' + (player.id - 10) % 26);
}

void ArenaView::render() const
{
    if (!_interactive)
        return;

    std::ostringstream out;
    out << "\033[2J\033[H" << BOLD << "=== ARENA COMMAND ===" << RESET;
    if (!_username.empty() && _hasState)
        out << "  " << _username << "  HP " << (_hp > 0 ? GREEN : RED) << _hp << RESET
            << "  pos (" << _x << "," << _y << ")  score " << _score;
    else
        out << "  " << DIM << "not logged in" << RESET;
    out << "\n\n    ";
    for (int x = 0; x < GRID_SIZE; ++x)
        out << (x % 10) << ' ';
    out << '\n';

    for (int y = 0; y < GRID_SIZE; ++y) {
        out << (y < 10 ? "  " : " ") << y << ' ';
        for (int x = 0; x < GRID_SIZE; ++x) {
            const RemotePlayer *cell = nullptr;
            for (const auto& p : _players)
                if (p.x == x && p.y == y && (!cell || cell->hp <= 0))
                    cell = &p;
            if (!cell)
                out << DIM << '.' << RESET << ' ';
            else if (cell->name == _username)
                out << BOLD << GREEN << '@' << RESET << ' ';
            else
                out << (cell->hp > 0 ? BOLD : RED) << symbolFor(*cell) << RESET << ' ';
        }
        out << '\n';
    }

    out << "\nPlayers:\n";
    for (const auto& p : _players)
        out << "  [" << p.id << "] " << p.name << " " << p.playerClass << "  " << p.hp << " HP  ("
            << p.x << "," << p.y << ")  score " << p.score << (p.name == _username ? "  <- you" : "") << '\n';

    out << "\n" << DIM << "--- messages ---" << RESET << '\n';
    for (const auto& line : _log)
        out << line << '\n';
    out << DIM << "w/a/s/d move · attack <id> · status · look · logout · help · quit" << RESET << "\n> ";
    std::cout << out.str() << std::flush;
}

std::vector<RemotePlayer> ArenaView::parseGameUpdate(const std::string& payload)
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
