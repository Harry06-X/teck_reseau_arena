/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Command — classe abstraite du pattern Command
*/

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include "game/GameContext.hpp"
#include "network/Address.hpp"

/*
 * Le thread réseau fabrique les commandes (CommandFactory) et les dépose
 * dans la CommandQueue ; le thread de jeu les exécute via run().
 */
class Command {
public:
    Command(uint32_t msgId, const net::ClientAddress& sender) : _msgId(msgId), _sender(sender) {}
    virtual ~Command() = default;

    // Template method : vérifie la session, met à jour l'activité, puis execute()
    void run(GameContext& ctx);

    virtual void execute(GameContext& ctx) = 0;

    // REGISTER et LOGIN surchargent pour accepter un client non connecté
    virtual bool requiresLogin() const { return true; }

    uint32_t getMsgId() const { return _msgId; }
    const net::ClientAddress& getSender() const { return _sender; }

protected:
    void reply(GameContext& ctx, const std::string& type, const std::vector<std::string>& fields) const;
    void ok(GameContext& ctx, const std::string& message) const;
    void error(GameContext& ctx, const std::string& message) const;

    // Envoie STATE|HP|X|Y|SCORE au joueur `entity`
    static void sendState(GameContext& ctx, ecs::Entity entity);

    net::ClientSession *session(GameContext& ctx) const;

    uint32_t _msgId;
    net::ClientAddress _sender;
};

#endif /* !COMMAND_HPP_ */
