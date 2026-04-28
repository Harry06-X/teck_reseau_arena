#include "../../include/command/MoveCommand.hpp"
MoveCommand::MoveCommand(std::size_t msgId, const std::string& direction)
    : _msgId(msgId), _direction(direction) {}
