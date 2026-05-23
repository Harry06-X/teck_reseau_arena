/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AttackCommand
*/

#ifndef ATTACKCOMMAND_HPP_
#define ATTACKCOMMAND_HPP_
#include "./Command.hpp"
#include <string>
#include <cstddef>
class AttackCommand : public Command {
    public:
        AttackCommand(std::size_t msgId, const std::string& targetId);
        ~AttackCommand();

        std::size_t getMsgId()const {return _msgId;}
        const std::string& getTargetId() const {return _targetId;}

    private:
        std::size_t _msgId;
        std::string _targetId;
};
#endif
