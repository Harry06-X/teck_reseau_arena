/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ArenaView — affichage terminal de l'arène 20x20
*/

#ifndef ARENAVIEW_HPP_
#define ARENAVIEW_HPP_

#include <cstddef>
#include "ClientState.hpp"

class ArenaView {
public:
    static constexpr std::size_t LOG_LINES = 8;

    // Efface l'écran et redessine la grille, les joueurs et le journal
    static void render(const ClientState& state);

private:
    static char symbolFor(const RemotePlayer& player);
};

#endif /* !ARENAVIEW_HPP_ */
