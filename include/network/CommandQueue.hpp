/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** CommandQueue
*/

#ifndef COMMANDQUEUE_HPP_
#define COMMANDQUEUE_HPP_

#include <queue>
#include <mutex>
#include <memory>
#include "command/Command.hpp"

namespace net {
    class CommandQueue {
    public:
        CommandQueue() = default;
        ~CommandQueue() = default;

        // Ajouté par TOI (Thread Réseau)
        void push(std::unique_ptr<Command> cmd);

        // Récupéré par TON BINÔME (Thread Game)
        // Retourne toutes les commandes d'un coup pour vider la file rapidement
        std::vector<std::unique_ptr<Command>> popAll();

    private:
        std::queue<std::unique_ptr<Command>> _queue;
        std::mutex _mutex; // Le verrou de sécurité
    };
}

#endif /* !COMMANDQUEUE_HPP_ */