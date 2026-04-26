/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** CommandQueue
*/

#include "../include/network/CommandQueue.hpp"

namespace net {

    void CommandQueue::push(std::unique_ptr<Command> cmd) {
        if (!cmd) return;
        
        // On verrouille pour que personne d'autre n'y touche
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(cmd));
    }

    std::vector<std::unique_ptr<Command>> CommandQueue::popAll() {
        std::vector<std::unique_ptr<Command>> commands;
        
        std::lock_guard<std::mutex> lock(_mutex);
        while (!_queue.empty()) {
            commands.push_back(std::move(_queue.front()));
            _queue.pop();
        }
        return commands;
    }

}