/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** GuiApp — client graphique SFML (bonus)
*/

#ifndef GUIAPP_HPP_
#define GUIAPP_HPP_

#include <SFML/Graphics.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
#include "ClientState.hpp"
#include "NetworkClient.hpp"

struct GuiOptions {
    std::string host = "127.0.0.1";
    uint16_t port = 4242;
    double packetLoss = 0.0;
    std::string username;          // connexion automatique si renseigné
    std::string password;
    bool archer = false;
    bool registerFirst = false;
    std::string screenshotPath;    // rendu hors écran puis sortie (documentation / tests)
    double screenshotDelay = 2.0;
    std::string executableDir;
};

class GuiApp {
public:
    static constexpr unsigned WIDTH = 1120;
    static constexpr unsigned HEIGHT = 760;

    explicit GuiApp(GuiOptions options);
    ~GuiApp() = default;

    GuiApp(const GuiApp&) = delete;
    GuiApp& operator=(const GuiApp&) = delete;

    int run();         // fenêtre interactive
    int screenshot();  // rendu hors écran dans options.screenshotPath

private:
    using Clock = std::chrono::steady_clock;

    // Réseau → GUI : les messages sont empilés par le thread réseau,
    // puis appliqués par le thread graphique à chaque frame.
    void onMessage(const net::ParsedMessage& msg);
    void processMessages();

    // Entrées
    void handleEvent(const sf::Event& event);
    void handleLoginEvent(const sf::Event& event);
    void handleGameEvent(const sf::Event& event);
    void submit(bool registerFirst);
    void attackNearest();
    void logout();

    // Rendu
    void draw(sf::RenderTarget& target);
    void drawHeader(sf::RenderTarget& target);
    void drawLogin(sf::RenderTarget& target);
    void drawBoard(sf::RenderTarget& target);
    void drawSidebar(sf::RenderTarget& target);
    void drawField(sf::RenderTarget& target, const sf::FloatRect& box, const std::string& label,
        const std::string& value, bool focused, bool masked);
    void drawButton(sf::RenderTarget& target, const sf::FloatRect& box, const std::string& label, bool active);
    void drawHpBar(sf::RenderTarget& target, float x, float y, float w, float h, int hp);
    void drawText(sf::RenderTarget& target, const std::string& text, float x, float y, unsigned size,
        sf::Color color, bool bold = false);

    std::optional<sf::Vector2i> cellAt(sf::Vector2i pixel) const;
    sf::Color colorFor(const RemotePlayer& player) const;
    void loadFonts();

    GuiOptions _options;
    sf::Font _font;
    sf::Font _bold;

    std::mutex _queueMutex;
    std::vector<net::ParsedMessage> _queue;
    ClientState _state;

    // Formulaire de connexion
    std::string _username;
    std::string _password;
    int _focus = 0; // 0 = username, 1 = password
    bool _archer = false;
    bool _loginAfterRegister = false;
    std::string _formMessage;
    bool _formError = false;

    sf::Vector2i _mouse{-1, -1};
    std::map<unsigned long, int> _lastHp;
    std::map<unsigned long, Clock::time_point> _hitAt;

    std::unique_ptr<NetworkClient> _network; // dernier membre : ses threads utilisent la file
};

#endif /* !GUIAPP_HPP_ */
