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

char ArenaView::symbolFor(const RemotePlayer& player)
{
    if (!player.alive())
        return 'x';
    if (player.id < 10)
        return static_cast<char>('0' + player.id);
    return static_cast<char>('A' + (player.id - 10) % 26);
}

void ArenaView::render(const ClientState& state)
{
    const RemotePlayer *me = state.self();
    std::ostringstream out;

    out << "\033[2J\033[H" << BOLD << "=== ARENA COMMAND ===" << RESET;
    if (state.loggedIn() && state.hasState())
        out << "  " << state.username() << "  HP " << (state.hp() > 0 ? GREEN : RED) << state.hp() << RESET
            << "  pos (" << state.x() << "," << state.y() << ")  score " << state.score();
    else
        out << "  " << DIM << "not logged in" << RESET;
    out << "\n\n    ";
    for (int x = 0; x < ClientState::GRID_SIZE; ++x)
        out << (x % 10) << ' ';
    out << '\n';

    for (int y = 0; y < ClientState::GRID_SIZE; ++y) {
        out << (y < 10 ? "  " : " ") << y << ' ';
        for (int x = 0; x < ClientState::GRID_SIZE; ++x) {
            const RemotePlayer *cell = state.playerAt(x, y);
            if (!cell)
                out << DIM << '.' << RESET << ' ';
            else if (cell == me)
                out << BOLD << GREEN << '@' << RESET << ' ';
            else
                out << (cell->alive() ? BOLD : RED) << symbolFor(*cell) << RESET << ' ';
        }
        out << '\n';
    }

    out << "\nPlayers:\n";
    for (const auto& p : state.players())
        out << "  [" << p.id << "] " << p.name << " " << p.playerClass << "  " << p.hp << " HP  ("
            << p.x << "," << p.y << ")  score " << p.score << (&p == me ? "  <- you" : "") << '\n';

    out << "\n" << DIM << "--- messages ---" << RESET << '\n';
    const auto& log = state.log();
    std::size_t start = log.size() > LOG_LINES ? log.size() - LOG_LINES : 0;
    for (std::size_t i = start; i < log.size(); ++i)
        out << log[i] << '\n';
    out << DIM << "w/a/s/d move · attack <id> · status · look · logout · help · quit" << RESET << "\n> ";
    std::cout << out.str() << std::flush;
}
