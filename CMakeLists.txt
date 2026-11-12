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

# 3. Cœur client partagé (réseau fiable + état de la partie)
add_library(arena_client_core STATIC
    client/common/NetworkClient.cpp
    client/common/ClientState.cpp
)
target_include_directories(arena_client_core PUBLIC client/common)
target_link_libraries(arena_client_core PUBLIC arena_common)
target_compile_options(arena_client_core PRIVATE -Wall -Wextra -Werror)

# 4. Client terminal (CLI)
add_executable(arena_client
    client/cli/main.cpp
    client/cli/Client.cpp
    client/cli/ArenaView.cpp
)
target_link_libraries(arena_client PRIVATE arena_client_core)
target_compile_options(arena_client PRIVATE -Wall -Wextra -Werror)

# 5. Client graphique SFML (bonus) — compilé seulement si SFML est installé
find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
if(SFML_FOUND)
    add_executable(arena_gui
        client/gui/main.cpp
        client/gui/GuiApp.cpp
    )
    target_link_libraries(arena_gui PRIVATE arena_client_core sfml-graphics sfml-window sfml-system)
    target_compile_options(arena_gui PRIVATE -Wall -Wextra -Werror)
else()
    message(WARNING "SFML not found: arena_gui will not be built (sudo apt install libsfml-dev)")
endif()
