/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Client — client terminal d'Arena Command
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <memory>
#include <mutex>
#include <string>
#include "ClientState.hpp"
#include "NetworkClient.hpp"

class Client {
public:
    Client(const std::string& host, uint16_t port, double packetLoss);
    ~Client() = default;

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // Boucle de lecture clavier (thread principal)
    void run();

private:
    bool handleInput(const std::string& line); // false = quitter
    void onMessage(const net::ParsedMessage& msg);
    void print(const std::string& line);
    void showHelp();

    bool _interactive;
    std::mutex _mutex; // protège _state (thread clavier + thread réseau)
    ClientState _state;
    std::unique_ptr<NetworkClient> _network; // déclaré en dernier : ses threads utilisent _state
};

#endif /* !CLIENT_HPP_ */
