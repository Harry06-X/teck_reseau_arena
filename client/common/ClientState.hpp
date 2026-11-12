/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ClientState — vue locale de la partie, construite à partir des messages serveur
*/

#ifndef CLIENTSTATE_HPP_
#define CLIENTSTATE_HPP_

#include <chrono>
#include <deque>
#include <optional>
#include <string>
#include <vector>
#include "network/ProtocolParser.hpp"

struct RemotePlayer {
    unsigned long id = 0;
    std::string name;
    std::string playerClass;
    int hp = 0;
    int x = 0;
    int y = 0;
    int score = 0;

    bool alive() const { return hp > 0; }
    int range() const { return playerClass == "ARCHER" ? 3 : 1; }
};

/*
 * Aucune logique réseau ni d'affichage ici : la CLI et la GUI appliquent les
 * messages reçus puis dessinent l'état chacune à leur manière.
 * Non thread-safe : l'appelant protège l'accès (mutex ou file de messages).
 */
class ClientState {
public:
    using Clock = std::chrono::steady_clock;
    static constexpr int GRID_SIZE = 20;
    static constexpr int RESPAWN_SECONDS = 10;
    static constexpr std::size_t MAX_LOG = 50;

    // verbose : STATE et LOOK produisent aussi une ligne de journal (mode non interactif)
    explicit ClientState(bool verbose = false) : _verbose(verbose) {}

    // Applique un message serveur ; retourne la ligne ajoutée au journal, s'il y en a une
    std::optional<std::string> apply(const net::ParsedMessage& msg);
    void addLog(const std::string& line);

    void setPendingLogin(const std::string& username) { _pendingUsername = username; }

    bool loggedIn() const { return _loggedIn; }
    const std::string& username() const { return _username; }
    std::optional<unsigned long> selfId() const { return _selfId; }
    bool hasState() const { return _hasState; }
    int hp() const { return _hp; }
    int x() const { return _x; }
    int y() const { return _y; }
    int score() const { return _score; }
    int respawnIn() const; // secondes avant réapparition (0 si vivant)

    const std::vector<RemotePlayer>& players() const { return _players; }
    const RemotePlayer *self() const;
    const RemotePlayer *playerAt(int x, int y) const;
    std::string nameOf(unsigned long id) const;
    const std::deque<std::string>& log() const { return _log; }

    // "id,nom,classe,hp,x,y,score;..." → joueurs
    static std::vector<RemotePlayer> parseGameUpdate(const std::string& payload);

private:
    void reset();

    bool _verbose;
    bool _loggedIn = false;
    std::string _pendingUsername;
    std::string _username;
    std::optional<unsigned long> _selfId;
    bool _hasState = false;
    int _hp = 0, _x = 0, _y = 0, _score = 0;
    std::optional<Clock::time_point> _diedAt;
    std::vector<RemotePlayer> _players;
    std::deque<std::string> _log;
};

#endif /* !CLIENTSTATE_HPP_ */
