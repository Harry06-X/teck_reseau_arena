/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Point d'entrée du client
*/

#include <iostream>
#include <stdexcept>
#include <string>
#include "Client.hpp"

namespace {
    void usage(const char *name)
    {
        std::cout << "USAGE: " << name << " [HOST] [PORT] [--loss RATIO]\n"
                  << "  HOST    server address (default 127.0.0.1)\n"
                  << "  PORT    server UDP port (default 4242)\n"
                  << "  --loss  simulated packet loss between 0 and 1 (default 0)\n";
    }
}

int main(int argc, char **argv)
{
    std::string host = "127.0.0.1";
    uint16_t port = 4242;
    double loss = 0.0;

    try {
        int positional = 0;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                usage(argv[0]);
                return 0;
            }
            if (arg == "--loss") {
                if (i + 1 >= argc)
                    throw std::invalid_argument("Missing value for --loss");
                loss = std::stod(argv[++i]);
            } else if (positional == 0) {
                host = arg;
                positional++;
            } else if (positional == 1) {
                port = static_cast<uint16_t>(std::stoi(arg));
                positional++;
            } else {
                throw std::invalid_argument("Unexpected argument " + arg);
            }
        }
        Client client(host, port, loss);
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "arena_client: " << e.what() << std::endl;
        usage(argv[0]);
        return 84;
    }
    return 0;
}
