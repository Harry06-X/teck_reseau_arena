cmake_minimum_required(VERSION 3.10)
project(ArenaCommand)

# Standard C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dossiers d'inclusion
include_directories(include)

# 2. Liste explicite de tous les fichiers sources
set(SOURCES
    # Core ECS
    src/ecs/Entity.cpp
    src/ecs/EntityManager.cpp
    src/ecs/ComponentManager.cpp
    src/ecs/SystemManager.cpp
    
    # Components
    src/components/HealthComponent.cpp
    src/components/PositionComponent.cpp
    src/components/ScoreComponent.cpp
    
    # Systems
    src/systems/CombatSystem.cpp
    src/systems/MovementSystem.cpp
    src/systems/RespawnSystem.cpp
    
    # Commands
    src/command/AttackCommand.cpp
    src/command/MoveCommand.cpp
    
    # Network & Entry Point
    src/network/UdpSocket.cpp
    src/network/ClientSession.cpp
    src/network/Server.cpp
)

# 3. Création de l'exécutable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(arena_server ${SOURCES})

# 4. Support des Threads
find_package(Threads REQUIRED)
target_link_libraries(arena_server PRIVATE Threads::Threads)

# 5. Flags de compilation
target_compile_options(arena_server PRIVATE -Wall -Wextra -Werror)