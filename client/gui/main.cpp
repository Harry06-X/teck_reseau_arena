/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Point d'entrée du client graphique
*/

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include "GuiApp.hpp"

namespace {
    void usage(const char *name)
    {
        std::cout << "USAGE: " << name << " [HOST] [PORT] [options]\n"
                  << "  HOST                 server address (default 127.0.0.1)\n"
                  << "  PORT                 server UDP port (default 4242)\n"
                  << "  --loss RATIO         simulated packet loss between 0 and 1\n"
                  << "  --user NAME --password PASS [--archer] [--register]\n"
                  << "                       log in automatically (optionally create the account first)\n"
                  << "  --screenshot FILE    render off-screen to a PNG after --delay seconds, then exit\n";
    }

    GuiOptions parseArgs(int argc, char **argv)
    {
        GuiOptions options;
        options.executableDir = std::filesystem::path(argv[0]).parent_path().string();
        int positional = 0;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            auto next = [&]() -> std::string {
                if (i + 1 >= argc)
                    throw std::invalid_argument("Missing value for " + arg);
                return argv[++i];
            };
            if (arg == "--loss")
                options.packetLoss = std::stod(next());
            else if (arg == "--user")
                options.username = next();
            else if (arg == "--password")
                options.password = next();
            else if (arg == "--archer")
                options.archer = true;
            else if (arg == "--register")
                options.registerFirst = true;
            else if (arg == "--screenshot")
                options.screenshotPath = next();
            else if (arg == "--delay")
                options.screenshotDelay = std::stod(next());
            else if (positional < 2 && arg[0] != '-') {
                if (positional++ == 0)
                    options.host = arg;
                else
                    options.port = static_cast<uint16_t>(std::stoi(arg));
            } else {
                throw std::invalid_argument("Unexpected argument " + arg);
            }
        }
        return options;
    }
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
            usage(argv[0]);
            return 0;
        }
    }
    try {
        GuiOptions options = parseArgs(argc, argv);
        GuiApp app(options);
        return options.screenshotPath.empty() ? app.run() : app.screenshot();
    } catch (const std::exception& e) {
        std::cerr << "arena_gui: " << e.what() << std::endl;
        usage(argv[0]);
        return 84;
    }
}
