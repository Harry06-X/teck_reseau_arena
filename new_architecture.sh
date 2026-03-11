#!/bin/bash

PROJECT_NAME="arena_command"

echo "Creating project architecture..."

mkdir -p $PROJECT_NAME/{include,src}
mkdir -p $PROJECT_NAME/include/{core,ecs,components,systems,network,command}
mkdir -p $PROJECT_NAME/src/{ecs,components,systems,network,command}

create_class() {
    HEADER_PATH=$1
    SOURCE_PATH=$2
    CLASS_NAME=$3

    # HEADER
    cat <<EOF > $HEADER_PATH
#pragma once

class $CLASS_NAME {
public:
    $CLASS_NAME();
    ~$CLASS_NAME();
};
EOF

    # SOURCE
    cat <<EOF > $SOURCE_PATH
#include "$(realpath --relative-to=$(dirname $SOURCE_PATH) $HEADER_PATH)"

$CLASS_NAME::$CLASS_NAME() {}

$CLASS_NAME::~$CLASS_NAME() {}
EOF
}

create_interface() {
    HEADER_PATH=$1
    CLASS_NAME=$2

    cat <<EOF > $HEADER_PATH
#pragma once

class $CLASS_NAME {
public:
    virtual ~$CLASS_NAME() = default;
};
EOF
}

# Core interfaces
create_interface "$PROJECT_NAME/include/core/ISystem.hpp" "ISystem"
create_interface "$PROJECT_NAME/include/core/IComponent.hpp" "IComponent"

# ECS
create_class "$PROJECT_NAME/include/ecs/Entity.hpp" "$PROJECT_NAME/src/ecs/Entity.cpp" "Entity"
create_class "$PROJECT_NAME/include/ecs/EntityManager.hpp" "$PROJECT_NAME/src/ecs/EntityManager.cpp" "EntityManager"
create_class "$PROJECT_NAME/include/ecs/ComponentManager.hpp" "$PROJECT_NAME/src/ecs/ComponentManager.cpp" "ComponentManager"
create_class "$PROJECT_NAME/include/ecs/SystemManager.hpp" "$PROJECT_NAME/src/ecs/SystemManager.cpp" "SystemManager"

# Components
create_class "$PROJECT_NAME/include/components/PositionComponent.hpp" "$PROJECT_NAME/src/components/PositionComponent.cpp" "PositionComponent"
create_class "$PROJECT_NAME/include/components/HealthComponent.hpp" "$PROJECT_NAME/src/components/HealthComponent.cpp" "HealthComponent"
create_class "$PROJECT_NAME/include/components/ScoreComponent.hpp" "$PROJECT_NAME/src/components/ScoreComponent.cpp" "ScoreComponent"

# Systems
create_class "$PROJECT_NAME/include/systems/MovementSystem.hpp" "$PROJECT_NAME/src/systems/MovementSystem.cpp" "MovementSystem"
create_class "$PROJECT_NAME/include/systems/CombatSystem.hpp" "$PROJECT_NAME/src/systems/CombatSystem.cpp" "CombatSystem"
create_class "$PROJECT_NAME/include/systems/RespawnSystem.hpp" "$PROJECT_NAME/src/systems/RespawnSystem.cpp" "RespawnSystem"

# Network
create_class "$PROJECT_NAME/include/network/Server.hpp" "$PROJECT_NAME/src/network/Server.cpp" "Server"
create_class "$PROJECT_NAME/include/network/ClientSession.hpp" "$PROJECT_NAME/src/network/ClientSession.cpp" "ClientSession"
create_class "$PROJECT_NAME/include/network/UdpSocket.hpp" "$PROJECT_NAME/src/network/UdpSocket.cpp" "UdpSocket"

# Command Pattern
create_interface "$PROJECT_NAME/include/command/Command.hpp" "Command"
create_class "$PROJECT_NAME/include/command/MoveCommand.hpp" "$PROJECT_NAME/src/command/MoveCommand.cpp" "MoveCommand"
create_class "$PROJECT_NAME/include/command/AttackCommand.hpp" "$PROJECT_NAME/src/command/AttackCommand.cpp" "AttackCommand"

echo "Architecture ECS created successfully."
