#include "../../include/command/AttackCommand.hpp"
AttackCommand::AttackCommand(std::size_t msgId, const std::string& targetId)
    : _msgId(msgId), _targetId(targetId) {}
