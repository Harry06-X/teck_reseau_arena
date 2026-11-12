/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** QueryCommands — STATUS, LOOK
*/

#ifndef QUERYCOMMANDS_HPP_
#define QUERYCOMMANDS_HPP_

#include "Command.hpp"

// STATUS → STATE|HP|X|Y|SCORE
class StatusCommand : public Command {
public:
    using Command::Command;
    ~StatusCommand() override = default;

    void execute(GameContext& ctx) override;
};

// LOOK → GAME_UPDATE|état complet de l'arène
class LookCommand : public Command {
public:
    using Command::Command;
    ~LookCommand() override = default;

    void execute(GameContext& ctx) override;
};

#endif /* !QUERYCOMMANDS_HPP_ */
