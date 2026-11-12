/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Server — serveur UDP autoritaire d'Arena Command
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include "auth/AccountManager.hpp"
#include "game/Game.hpp"
#include "game/GameContext.hpp"
#include "network/ClientSession.hpp"
#include "network/CommandQueue.hpp"
#include "network/ReliabilityLayer.hpp"
#include "network/UdpSocket.hpp"

namespace net {

    struct ServerConfig {
        uint16_t port = 4242;
        std::string accountsFile = "users.db";
        double packetLoss = 0.0;                         // simulation de pertes (tests)
        std::chrono::seconds sessionTimeout = ClientSession::DEFAULT_TIMEOUT;
        unsigned int seed = std::random_device{}();
    };

    /*
     * Trois threads :
     *   - réseau      : reçoit, ACK, filtre les doublons, parse → CommandQueue
     *   - maintenance : retransmet les messages non acquittés
     *   - jeu (run)   : exécute les commandes, respawn, timeouts, GAME_UPDATE
     * Game et SessionManager ne sont manipulés que par le thread de jeu.
     */
    class Server : public IResponder {
    public:
        static constexpr std::chrono::milliseconds TICK{20};
        static constexpr std::chrono::milliseconds UPDATE_MIN_INTERVAL{100};
        static constexpr std::chrono::milliseconds UPDATE_KEEPALIVE{2000};

        explicit Server(const ServerConfig& config);
        ~Server() override;

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;

        void run();   // bloque jusqu'à stop()
        void stop();  // async-signal-safe (simple écriture atomique)

        // IResponder
        void reply(const ClientAddress& to, const std::string& type,
            const std::vector<std::string>& fields) override;
        void broadcast(const std::string& type, const std::vector<std::string>& fields) override;

    private:
        using Clock = std::chrono::steady_clock;

        void networkLoop();
        void maintenanceLoop();
        void gameTick();
        void handleDatagram(const std::string& raw, const ClientAddress& from);
        void sendUnreliable(const ClientAddress& to, const std::string& type, const std::vector<std::string>& fields);
        void sendState(ecs::Entity entity);
        void closeSession(const ClientAddress& address, const std::string& reason);

        ServerConfig _config;
        UdpSocket _socket;
        ReliabilityLayer _reliability;
        CommandQueue _queue;
        Game _game;
        AccountManager _accounts;
        SessionManager _sessions;
        GameContext _ctx;

        std::atomic<bool> _running{false};
        std::thread _networkThread;
        std::thread _maintenanceThread;
        Clock::time_point _lastUpdate;
    };
}

#endif /* !SERVER_HPP_ */
