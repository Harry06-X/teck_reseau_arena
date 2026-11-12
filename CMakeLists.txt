cmake_minimum_required(VERSION 3.10)
project(ArenaCommand LANGUAGES CXX)

# Standard C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Les binaires sont générés à la racine du dépôt
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# 1. Bibliothèque commune client/serveur : socket, protocole, fiabilité
add_library(arena_common STATIC
    src/network/Address.cpp
    src/network/UdpSocket.cpp
    src/network/ProtocolParser.cpp
    src/network/ReliabilityLayer.cpp
)
target_include_directories(arena_common PUBLIC include)
target_link_libraries(arena_common PUBLIC Threads::Threads)
target_compile_options(arena_common PRIVATE -Wall -Wextra -Werror)

# 2. Serveur
add_executable(arena_server
    src/server_main.cpp
    # ECS
    src/ecs/EntityManager.cpp
    src/ecs/ComponentManager.cpp
    # Systems
    src/systems/MovementSystem.cpp
    src/systems/CombatSystem.cpp
    src/systems/RespawnSystem.cpp
    # Game & auth
    src/game/Game.cpp
    src/auth/AccountManager.cpp
    # Commands
    src/command/Command.cpp
    src/command/CommandFactory.cpp
    src/command/MoveCommand.cpp
    src/command/AttackCommand.cpp
    src/command/AuthCommands.cpp
    src/command/QueryCommands.cpp
    # Network
    src/network/ClientSession.cpp
    src/network/CommandQueue.cpp
    src/network/Server.cpp
)
target_link_libraries(arena_server PRIVATE arena_common OpenSSL::Crypto)
target_compile_options(arena_server PRIVATE -Wall -Wextra -Werror)

# 3. Client terminal
add_executable(arena_client
    client/main.cpp
    client/Client.cpp
    client/ArenaView.cpp
)
target_include_directories(arena_client PRIVATE client)
target_link_libraries(arena_client PRIVATE arena_common)
target_compile_options(arena_client PRIVATE -Wall -Wextra -Werror)
