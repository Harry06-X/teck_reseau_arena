/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Point d'entrée du serveur
*/

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <string>
#include "network/Server.hpp"

namespace {
    net::Server *g_server = nullptr;

    void onSignal(int)
    {
        if (g_server)
            g_server->stop();
    }

    void usage(const char *name)
    {
        std::cout << "USAGE: " << name << " [-p PORT] [--users FILE] [--timeout SECONDS] [--loss RATIO] [--seed N]\n"
                  << "  -p, --port      UDP port to listen on (default 4242)\n"
                  << "  --users         accounts file (default users.db)\n"
                  << "  --timeout       inactivity timeout in seconds (default 60)\n"
                  << "  --loss          simulated packet loss between 0 and 1 (default 0)\n"
                  << "  --seed          random seed for spawn positions\n";
    }

    net::ServerConfig parseArgs(int argc, char **argv)
    {
        net::ServerConfig config;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            auto next = [&]() -> std::string {
                if (i + 1 >= argc)
                    throw std::invalid_argument("Missing value for " + arg);
                return argv[++i];
            };
            if (arg == "-p" || arg == "--port")
                config.port = static_cast<uint16_t>(std::stoi(next()));
            else if (arg == "--users")
                config.accountsFile = next();
            else if (arg == "--timeout")
                config.sessionTimeout = std::chrono::seconds(std::stoi(next()));
            else if (arg == "--loss")
                config.packetLoss = std::stod(next());
            else if (arg == "--seed")
                config.seed = static_cast<unsigned int>(std::stoul(next()));
            else
                throw std::invalid_argument("Unknown option " + arg);
        }
        if (config.packetLoss < 0.0 || config.packetLoss >= 1.0)
            throw std::invalid_argument("--loss must be in [0, 1)");
        return config;
    }
}

int main(int argc, char **argv)
{
    if (argc > 1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        usage(argv[0]);
        return 0;
    }
    try {
        net::Server server(parseArgs(argc, argv));
        g_server = &server;
        std::signal(SIGINT, onSignal);
        std::signal(SIGTERM, onSignal);
        server.run();
        g_server = nullptr;
    } catch (const std::exception& e) {
        std::cerr << "arena_server: " << e.what() << std::endl;
        usage(argv[0]);
        return 84;
    }
    return 0;
}
