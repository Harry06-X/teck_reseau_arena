/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** GameContext — ce dont une commande a besoin pour s'exécuter
*/

#ifndef GAMECONTEXT_HPP_
#define GAMECONTEXT_HPP_

#include <string>
#include <vector>
#include "auth/AccountManager.hpp"
#include "game/Game.hpp"
#include "network/ClientSession.hpp"

// Abstraction de l'envoi : les commandes ne connaissent pas le socket
class IResponder {
public:
    virtual ~IResponder() = default;

    // Message fiable (avec MSGID + ACK attendu) vers un client
    virtual void reply(const net::ClientAddress& to, const std::string& type,
        const std::vector<std::string>& fields) = 0;

    // Message fiable vers tous les joueurs connectés
    virtual void broadcast(const std::string& type, const std::vector<std::string>& fields) = 0;
};

struct GameContext {
    Game& game;
    AccountManager& accounts;
    net::SessionManager& sessions;
    IResponder& responder;
};

#endif /* !GAMECONTEXT_HPP_ */
