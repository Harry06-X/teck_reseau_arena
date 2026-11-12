/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ArenaView — affichage terminal de l'arène 20x20
*/

#ifndef ARENAVIEW_HPP_
#define ARENAVIEW_HPP_

#include <deque>
#include <string>
#include <vector>

struct RemotePlayer {
    unsigned long id = 0;
    std::string name;
    std::string playerClass;
    int hp = 0;
    int x = 0;
    int y = 0;
    int score = 0;
};

class ArenaView {
public:
    static constexpr int GRID_SIZE = 20;
    static constexpr std::size_t LOG_LINES = 8;

    // interactive = terminal : on efface l'écran et on redessine tout
    explicit ArenaView(bool interactive);

    void setUsername(const std::string& username);
    void setSelfState(int hp, int x, int y, int score);
    void setPlayers(std::vector<RemotePlayer> players);
    void log(const std::string& line);
    std::string nameOf(unsigned long id) const;

    void render() const;

    // "id,nom,classe,hp,x,y,score;..." → joueurs
    static std::vector<RemotePlayer> parseGameUpdate(const std::string& payload);

private:
    static char symbolFor(const RemotePlayer& player);

    bool _interactive;
    std::string _username;
    bool _hasState = false;
    int _hp = 0, _x = 0, _y = 0, _score = 0;
    std::vector<RemotePlayer> _players;
    std::deque<std::string> _log;
};

#endif /* !ARENAVIEW_HPP_ */
