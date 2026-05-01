/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** MoveCommand
*/

#ifndef MOVECOMMAND_HPP_
#define MOVECOMMAND_HPP_
#include "./Command.hpp"
#include <string>
#include <cstddef>
class MoveCommand : public Command {
    public:
        MoveCommand(std::size_t msgId, const std::string& direction);
        ~MoveCommand();

        std::size_t getMsgId()const { return _msgId; }
        const std::string& getDirection() const { return _direction; }

    private:
        std::size_t _msgId;
        std::string _direction;
};
#endif
