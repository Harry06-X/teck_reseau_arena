/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Client — client terminal d'Arena Command
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "ArenaView.hpp"
#include "network/ProtocolParser.hpp"
#include "network/ReliabilityLayer.hpp"
#include "network/UdpSocket.hpp"

class Client {
public:
    Client(const std::string& host, uint16_t port, double packetLoss);
    ~Client();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // Boucle de lecture clavier (thread principal)
    void run();

private:
    void receiveLoop();
    void maintenanceLoop();
    bool handleInput(const std::string& line); // false = quitter
    void sendCommand(const std::string& type, const std::vector<std::string>& fields = {});
    void handleMessage(const net::ParsedMessage& msg);
    void print(const std::string& line);
    void showHelp();

    net::UdpSocket _socket;
    net::ClientAddress _server;
    net::ReliabilityLayer _reliability;

    ArenaView _view;
    std::mutex _viewMutex;
    std::string _pendingUsername;

    std::atomic<bool> _running{true};
    std::atomic<bool> _loggedIn{false};
    std::thread _receiver;
    std::thread _maintenance;
};

#endif /* !CLIENT_HPP_ */
