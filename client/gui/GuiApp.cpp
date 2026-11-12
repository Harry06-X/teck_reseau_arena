/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** GuiApp
*/

#include "GuiApp.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <thread>

namespace {
    // Palette
    const sf::Color BG(18, 20, 26);
    const sf::Color PANEL(28, 31, 40);
    const sf::Color PANEL_LIGHT(38, 42, 54);
    const sf::Color GRID(44, 49, 62);
    const sf::Color TEXT(229, 231, 235);
    const sf::Color DIM(148, 155, 168);
    const sf::Color ACCENT(245, 158, 11);
    const sf::Color SELF(74, 222, 128);
    const sf::Color DANGER(239, 68, 68);
    const sf::Color DEAD(90, 96, 110);

    // Mise en page
    constexpr float HEADER_H = 56.f;
    constexpr float BOARD_X = 32.f;
    constexpr float BOARD_Y = 84.f;
    constexpr float CELL = 32.f;
    constexpr float BOARD_SIZE = CELL * ClientState::GRID_SIZE;
    constexpr float SIDE_X = BOARD_X + BOARD_SIZE + 36.f;
    constexpr float SIDE_W = GuiApp::WIDTH - SIDE_X - 32.f;

    // Formulaire de connexion (centré)
    constexpr float FORM_W = 420.f;
    constexpr float FORM_X = (GuiApp::WIDTH - FORM_W) / 2.f;
    constexpr float FORM_Y = 150.f;
    const sf::FloatRect USER_BOX(FORM_X + 32, FORM_Y + 120, FORM_W - 64, 44);
    const sf::FloatRect PASS_BOX(FORM_X + 32, FORM_Y + 200, FORM_W - 64, 44);
    const sf::FloatRect WARRIOR_BOX(FORM_X + 32, FORM_Y + 280, (FORM_W - 72) / 2, 40);
    const sf::FloatRect ARCHER_BOX(FORM_X + 40 + (FORM_W - 72) / 2, FORM_Y + 280, (FORM_W - 72) / 2, 40);
    const sf::FloatRect LOGIN_BOX(FORM_X + 32, FORM_Y + 344, (FORM_W - 72) / 2, 46);
    const sf::FloatRect REGISTER_BOX(FORM_X + 40 + (FORM_W - 72) / 2, FORM_Y + 344, (FORM_W - 72) / 2, 46);

    sf::String utf8(const std::string& s)
    {
        return sf::String::fromUtf8(s.begin(), s.end());
    }

    bool contains(const sf::FloatRect& box, sf::Vector2i p)
    {
        return box.contains(static_cast<float>(p.x), static_cast<float>(p.y));
    }
}

GuiApp::GuiApp(GuiOptions options) : _options(std::move(options))
{
    loadFonts();
    _username = _options.username;
    _password = _options.password;
    _archer = _options.archer;
    _network = std::make_unique<NetworkClient>(_options.host, _options.port, _options.packetLoss,
        [this](const net::ParsedMessage& msg) { onMessage(msg); });
    if (!_username.empty() && !_password.empty())
        submit(_options.registerFirst);
}

void GuiApp::loadFonts()
{
    const std::vector<std::string> dirs = {
        "assets/fonts/",
        _options.executableDir + "/assets/fonts/",
        "/usr/share/fonts/truetype/dejavu/"
    };
    for (const auto& dir : dirs) {
        if (std::filesystem::exists(dir + "DejaVuSans.ttf") && _font.loadFromFile(dir + "DejaVuSans.ttf")) {
            if (!_bold.loadFromFile(dir + "DejaVuSans-Bold.ttf"))
                _bold = _font;
            return;
        }
    }
    throw std::runtime_error("Font DejaVuSans.ttf not found (run from the repository root)");
}

// ---------------------------------------------------------------- boucles

int GuiApp::run()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arena Command", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else
                handleEvent(event);
        }
        processMessages();
        window.clear(BG);
        draw(window);
        window.display();
    }
    logout();
    return 0;
}

int GuiApp::screenshot()
{
    sf::RenderTexture texture;
    if (!texture.create(WIDTH, HEIGHT))
        throw std::runtime_error("Cannot create render texture");

    const auto end = Clock::now() + std::chrono::milliseconds(static_cast<int>(_options.screenshotDelay * 1000));
    while (Clock::now() < end) {
        processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    processMessages();
    texture.clear(BG);
    draw(texture);
    texture.display();
    if (!texture.getTexture().copyToImage().saveToFile(_options.screenshotPath))
        throw std::runtime_error("Cannot save " + _options.screenshotPath);
    logout();
    return 0;
}

void GuiApp::logout()
{
    processMessages();
    if (!_state.loggedIn())
        return;
    _network->send("LOGOUT");
    for (int i = 0; i < 40 && _state.loggedIn(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        processMessages();
    }
}

// ---------------------------------------------------------------- réseau

void GuiApp::onMessage(const net::ParsedMessage& msg)
{
    std::lock_guard<std::mutex> lock(_queueMutex);
    _queue.push_back(msg);
}

void GuiApp::processMessages()
{
    std::vector<net::ParsedMessage> pending;
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        pending.swap(_queue);
    }

    for (const auto& msg : pending) {
        auto line = _state.apply(msg);
        const std::string text = msg.fields.empty() ? "" : msg.fields[0];

        // Retour visuel sur l'écran de connexion
        if (!_state.loggedIn() && line && (msg.type == "OK" || msg.type == "ERROR" || msg.type == "UNREACHABLE")) {
            _formMessage = text;
            _formError = msg.type != "OK";
        }
        // "Register & play" : connexion automatique une fois le compte créé
        if (_loginAfterRegister && msg.type == "OK" && text.rfind("Account created", 0) == 0) {
            _loginAfterRegister = false;
            submit(false);
        } else if (_loginAfterRegister && msg.type == "ERROR") {
            _loginAfterRegister = false;
        }

        // Flash rouge sur les joueurs qui viennent de perdre des HP
        if (msg.type == "GAME_UPDATE") {
            for (const auto& p : _state.players()) {
                auto it = _lastHp.find(p.id);
                if (it != _lastHp.end() && p.hp < it->second)
                    _hitAt[p.id] = Clock::now();
                _lastHp[p.id] = p.hp;
            }
        }
    }
}

// ---------------------------------------------------------------- entrées

void GuiApp::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseMoved)
        _mouse = {event.mouseMove.x, event.mouseMove.y};
    if (_state.loggedIn())
        handleGameEvent(event);
    else
        handleLoginEvent(event);
}

void GuiApp::handleLoginEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i p(event.mouseButton.x, event.mouseButton.y);
        if (contains(USER_BOX, p))
            _focus = 0;
        else if (contains(PASS_BOX, p))
            _focus = 1;
        else if (contains(WARRIOR_BOX, p))
            _archer = false;
        else if (contains(ARCHER_BOX, p))
            _archer = true;
        else if (contains(LOGIN_BOX, p))
            submit(false);
        else if (contains(REGISTER_BOX, p))
            submit(true);
    } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tab)
            _focus = 1 - _focus;
        else if (event.key.code == sf::Keyboard::Enter)
            submit(false);
    } else if (event.type == sf::Event::TextEntered) {
        std::string& field = _focus == 0 ? _username : _password;
        const sf::Uint32 c = event.text.unicode;
        if (c == 8 && !field.empty())
            field.pop_back();
        else if (c >= 0x20 && c < 0x7f && c != '|' && field.size() < (_focus == 0 ? 16u : 32u))
            field.push_back(static_cast<char>(c));
    }
}

void GuiApp::handleGameEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up: case sf::Keyboard::W: case sf::Keyboard::Z:
                _network->send("MOVE", {"UP"});
                break;
            case sf::Keyboard::Down: case sf::Keyboard::S:
                _network->send("MOVE", {"DOWN"});
                break;
            case sf::Keyboard::Left: case sf::Keyboard::A: case sf::Keyboard::Q:
                _network->send("MOVE", {"LEFT"});
                break;
            case sf::Keyboard::Right: case sf::Keyboard::D:
                _network->send("MOVE", {"RIGHT"});
                break;
            case sf::Keyboard::Space:
                attackNearest();
                break;
            case sf::Keyboard::L:
                _network->send("LOOK");
                break;
            case sf::Keyboard::Escape:
                _network->send("LOGOUT");
                break;
            default:
                break;
        }
    } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        auto cell = cellAt({event.mouseButton.x, event.mouseButton.y});
        if (!cell)
            return;
        const RemotePlayer *target = _state.playerAt(cell->x, cell->y);
        if (target && target != _state.self())
            _network->send("ATTACK", {std::to_string(target->id)});
    }
}

void GuiApp::submit(bool registerFirst)
{
    if (!net::ProtocolParser::isValidUsername(_username)) {
        _formMessage = "Username: 3-16 characters [A-Za-z0-9_]";
        _formError = true;
        return;
    }
    if (!net::ProtocolParser::isValidPassword(_password)) {
        _formMessage = "Password: 4-32 printable characters";
        _formError = true;
        return;
    }
    _formError = false;
    if (registerFirst) {
        _loginAfterRegister = true;
        _formMessage = "Creating account...";
        _network->send("REGISTER", {_username, _password});
        return;
    }
    _formMessage = "Connecting...";
    _state.setPendingLogin(_username);
    _network->send("LOGIN", {_username, _password, _archer ? "ARCHER" : "WARRIOR"});
}

void GuiApp::attackNearest()
{
    const RemotePlayer *me = _state.self();
    if (!me)
        return;
    const RemotePlayer *best = nullptr;
    int bestDistance = 1000;
    for (const auto& p : _state.players()) {
        if (&p == me || !p.alive())
            continue;
        int distance = std::max(std::abs(p.x - me->x), std::abs(p.y - me->y));
        if (distance <= me->range() && distance < bestDistance) {
            best = &p;
            bestDistance = distance;
        }
    }
    if (best)
        _network->send("ATTACK", {std::to_string(best->id)});
    else
        _state.addLog("✖ No enemy in range (" + std::to_string(me->range()) + ")");
}

std::optional<sf::Vector2i> GuiApp::cellAt(sf::Vector2i pixel) const
{
    float fx = (static_cast<float>(pixel.x) - BOARD_X) / CELL;
    float fy = (static_cast<float>(pixel.y) - BOARD_Y) / CELL;
    if (fx < 0 || fy < 0 || fx >= ClientState::GRID_SIZE || fy >= ClientState::GRID_SIZE)
        return std::nullopt;
    return sf::Vector2i(static_cast<int>(fx), static_cast<int>(fy));
}

// ---------------------------------------------------------------- rendu

void GuiApp::draw(sf::RenderTarget& target)
{
    drawHeader(target);
    if (_state.loggedIn()) {
        drawBoard(target);
        drawSidebar(target);
    } else {
        drawLogin(target);
    }
}

void GuiApp::drawText(sf::RenderTarget& target, const std::string& text, float x, float y, unsigned size,
    sf::Color color, bool bold)
{
    sf::Text t(utf8(text), bold ? _bold : _font, size);
    t.setFillColor(color);
    t.setPosition(std::round(x), std::round(y));
    target.draw(t);
}

void GuiApp::drawHeader(sf::RenderTarget& target)
{
    sf::RectangleShape bar({static_cast<float>(WIDTH), HEADER_H});
    bar.setFillColor(PANEL);
    target.draw(bar);
    drawText(target, "⚔ ARENA COMMAND", 32, 14, 22, ACCENT, true);
    std::string right = "Server " + _network->serverAddress();
    if (_state.loggedIn())
        right = _state.username() + "  ·  " + std::to_string(_state.players().size()) + " in arena  ·  " + right;
    sf::Text t(utf8(right), _font, 15);
    t.setFillColor(DIM);
    t.setPosition(std::round(WIDTH - 32 - t.getLocalBounds().width), 20);
    target.draw(t);
}

void GuiApp::drawField(sf::RenderTarget& target, const sf::FloatRect& box, const std::string& label,
    const std::string& value, bool focused, bool masked)
{
    drawText(target, label, box.left, box.top - 24, 14, DIM);
    sf::RectangleShape field({box.width, box.height});
    field.setPosition(box.left, box.top);
    field.setFillColor(BG);
    field.setOutlineThickness(2);
    field.setOutlineColor(focused ? ACCENT : GRID);
    target.draw(field);
    std::string shown = masked ? std::string(value.size(), '*') : value;
    bool caret = focused && (std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now().time_since_epoch()).count() / 500) % 2 == 0;
    drawText(target, shown + (caret ? "|" : ""), box.left + 12, box.top + 10, 18, TEXT);
}

void GuiApp::drawButton(sf::RenderTarget& target, const sf::FloatRect& box, const std::string& label, bool active)
{
    sf::RectangleShape button({box.width, box.height});
    button.setPosition(box.left, box.top);
    bool hover = contains(box, _mouse);
    button.setFillColor(active ? ACCENT : (hover ? PANEL_LIGHT : PANEL));
    button.setOutlineThickness(2);
    button.setOutlineColor(active ? ACCENT : GRID);
    target.draw(button);
    sf::Text t(utf8(label), _bold, 16);
    t.setFillColor(active ? BG : TEXT);
    auto bounds = t.getLocalBounds();
    t.setPosition(std::round(box.left + (box.width - bounds.width) / 2 - bounds.left),
        std::round(box.top + (box.height - bounds.height) / 2 - bounds.top));
    target.draw(t);
}

void GuiApp::drawLogin(sf::RenderTarget& target)
{
    sf::RectangleShape panel({FORM_W, 470});
    panel.setPosition(FORM_X, FORM_Y);
    panel.setFillColor(PANEL);
    panel.setOutlineThickness(1);
    panel.setOutlineColor(GRID);
    target.draw(panel);

    drawText(target, "Join the arena", FORM_X + 32, FORM_Y + 28, 26, TEXT, true);
    drawText(target, "Multiplayer UDP game · 20×20 arena · 100 HP", FORM_X + 32, FORM_Y + 66, 14, DIM);

    drawField(target, USER_BOX, "Username", _username, _focus == 0, false);
    drawField(target, PASS_BOX, "Password", _password, _focus == 1, true);
    drawText(target, "Class", WARRIOR_BOX.left, WARRIOR_BOX.top - 24, 14, DIM);
    drawButton(target, WARRIOR_BOX, "Warrior · range 1", !_archer);
    drawButton(target, ARCHER_BOX, "Archer · range 3", _archer);
    drawButton(target, LOGIN_BOX, "Login", false);
    drawButton(target, REGISTER_BOX, "Register & play", false);

    if (!_formMessage.empty())
        drawText(target, (_formError ? "✖ " : "✔ ") + _formMessage, FORM_X + 32, FORM_Y + 406, 15,
            _formError ? DANGER : SELF);
    drawText(target, "Tab: switch field · Enter: login", FORM_X + 32, FORM_Y + 436, 13, DIM);
}

sf::Color GuiApp::colorFor(const RemotePlayer& player) const
{
    static const std::vector<sf::Color> palette = {
        {96, 165, 250}, {244, 114, 182}, {250, 204, 21}, {167, 139, 250},
        {45, 212, 191}, {251, 146, 60}, {248, 113, 113}, {163, 230, 53}
    };
    if (!player.alive())
        return DEAD;
    if (&player == _state.self())
        return SELF;
    return palette[player.id % palette.size()];
}

void GuiApp::drawHpBar(sf::RenderTarget& target, float x, float y, float w, float h, int hp)
{
    sf::RectangleShape back({w, h});
    back.setPosition(x, y);
    back.setFillColor(sf::Color(55, 60, 75));
    target.draw(back);
    float ratio = std::clamp(hp / 100.f, 0.f, 1.f);
    sf::RectangleShape fill({w * ratio, h});
    fill.setPosition(x, y);
    fill.setFillColor(ratio > 0.5f ? SELF : (ratio > 0.25f ? ACCENT : DANGER));
    target.draw(fill);
}

void GuiApp::drawBoard(sf::RenderTarget& target)
{
    const RemotePlayer *me = _state.self();

    sf::RectangleShape board({BOARD_SIZE, BOARD_SIZE});
    board.setPosition(BOARD_X, BOARD_Y);
    board.setFillColor(PANEL);
    target.draw(board);

    // Zone de portée du joueur
    if (me && me->alive()) {
        int r = me->range();
        int x0 = std::max(0, me->x - r), y0 = std::max(0, me->y - r);
        int x1 = std::min(ClientState::GRID_SIZE - 1, me->x + r), y1 = std::min(ClientState::GRID_SIZE - 1, me->y + r);
        sf::RectangleShape zone({(x1 - x0 + 1) * CELL, (y1 - y0 + 1) * CELL});
        zone.setPosition(BOARD_X + x0 * CELL, BOARD_Y + y0 * CELL);
        zone.setFillColor(sf::Color(245, 158, 11, 28));
        target.draw(zone);
    }

    // Grille
    sf::RectangleShape line;
    line.setFillColor(GRID);
    for (int i = 0; i <= ClientState::GRID_SIZE; ++i) {
        line.setSize({1, BOARD_SIZE});
        line.setPosition(BOARD_X + i * CELL, BOARD_Y);
        target.draw(line);
        line.setSize({BOARD_SIZE, 1});
        line.setPosition(BOARD_X, BOARD_Y + i * CELL);
        target.draw(line);
    }
    for (int i = 0; i < ClientState::GRID_SIZE; i += 5) {
        drawText(target, std::to_string(i), BOARD_X + i * CELL + 3, BOARD_Y - 20, 12, DIM);
        drawText(target, std::to_string(i), BOARD_X - 22, BOARD_Y + i * CELL + 8, 12, DIM);
    }

    // Case survolée
    if (auto cell = cellAt(_mouse)) {
        const RemotePlayer *hovered = _state.playerAt(cell->x, cell->y);
        bool attackable = hovered && hovered != me && hovered->alive();
        sf::RectangleShape hover({CELL - 2, CELL - 2});
        hover.setPosition(BOARD_X + cell->x * CELL + 1, BOARD_Y + cell->y * CELL + 1);
        hover.setFillColor(sf::Color::Transparent);
        hover.setOutlineThickness(2);
        hover.setOutlineColor(attackable ? DANGER : DIM);
        target.draw(hover);
    }

    // Joueurs (les morts d'abord, les vivants par-dessus)
    std::vector<const RemotePlayer *> order;
    for (const auto& p : _state.players())
        order.push_back(&p);
    std::stable_sort(order.begin(), order.end(), [](auto a, auto b) { return !a->alive() && b->alive(); });

    for (const RemotePlayer *p : order) {
        float cx = BOARD_X + p->x * CELL + CELL / 2;
        float cy = BOARD_Y + p->y * CELL + CELL / 2;
        auto hit = _hitAt.find(p->id);
        bool flashing = hit != _hitAt.end() && Clock::now() - hit->second < std::chrono::milliseconds(350);

        sf::CircleShape token(CELL / 2 - 4);
        token.setOrigin(token.getRadius(), token.getRadius());
        token.setPosition(cx, cy + 2);
        token.setFillColor(colorFor(*p));
        token.setOutlineThickness(flashing ? 3 : (p == me ? 2 : 0));
        token.setOutlineColor(flashing ? DANGER : TEXT);
        target.draw(token);

        sf::Text label(utf8(p->alive() ? std::to_string(p->id) : "✖"), _bold, 13);
        label.setFillColor(BG);
        auto b = label.getLocalBounds();
        label.setPosition(std::round(cx - b.width / 2 - b.left), std::round(cy + 2 - b.height / 2 - b.top));
        target.draw(label);

        if (p->alive())
            drawHpBar(target, cx - CELL / 2 + 4, BOARD_Y + p->y * CELL + 2, CELL - 8, 3, p->hp);
    }

    // Écran de mort
    if (_state.hasState() && _state.hp() <= 0) {
        sf::RectangleShape veil({BOARD_SIZE, BOARD_SIZE});
        veil.setPosition(BOARD_X, BOARD_Y);
        veil.setFillColor(sf::Color(10, 10, 14, 170));
        target.draw(veil);
        sf::Text title(utf8("☠ You were eliminated"), _bold, 30);
        title.setFillColor(DANGER);
        auto b = title.getLocalBounds();
        title.setPosition(std::round(BOARD_X + (BOARD_SIZE - b.width) / 2), BOARD_Y + BOARD_SIZE / 2 - 40);
        target.draw(title);
        int left = _state.respawnIn();
        sf::Text sub(utf8(left > 0 ? "Respawn in " + std::to_string(left) + "s" : "Respawning..."), _font, 18);
        sub.setFillColor(TEXT);
        b = sub.getLocalBounds();
        sub.setPosition(std::round(BOARD_X + (BOARD_SIZE - b.width) / 2), BOARD_Y + BOARD_SIZE / 2 + 6);
        target.draw(sub);
    }
}

void GuiApp::drawSidebar(sf::RenderTarget& target)
{
    const RemotePlayer *me = _state.self();
    float y = BOARD_Y;

    // Fiche du joueur
    sf::RectangleShape card({SIDE_W, 132});
    card.setPosition(SIDE_X, y);
    card.setFillColor(PANEL);
    target.draw(card);
    drawText(target, _state.username(), SIDE_X + 18, y + 14, 22, SELF, true);
    if (me)
        drawText(target, me->playerClass + " · range " + std::to_string(me->range()) + " · id "
            + std::to_string(me->id), SIDE_X + 18, y + 46, 14, DIM);
    drawText(target, "HP", SIDE_X + 18, y + 76, 14, DIM);
    drawHpBar(target, SIDE_X + 52, y + 80, SIDE_W - 140, 12, _state.hp());
    drawText(target, std::to_string(_state.hp()) + " / 100", SIDE_X + SIDE_W - 80, y + 74, 15, TEXT, true);
    drawText(target, "Position (" + std::to_string(_state.x()) + ", " + std::to_string(_state.y()) + ")",
        SIDE_X + 18, y + 102, 15, TEXT);
    drawText(target, "Score " + std::to_string(_state.score()), SIDE_X + SIDE_W - 96, y + 102, 15, ACCENT, true);
    y += 148;

    // Classement
    drawText(target, "PLAYERS", SIDE_X, y, 13, DIM, true);
    y += 22;
    std::vector<const RemotePlayer *> ranking;
    for (const auto& p : _state.players())
        ranking.push_back(&p);
    std::sort(ranking.begin(), ranking.end(), [](auto a, auto b) {
        return a->score != b->score ? a->score > b->score : a->id < b->id;
    });
    std::size_t shown = 0;
    for (const RemotePlayer *p : ranking) {
        if (shown++ >= 8)
            break;
        sf::RectangleShape row({SIDE_W, 26});
        row.setPosition(SIDE_X, y);
        row.setFillColor(p == me ? PANEL_LIGHT : PANEL);
        target.draw(row);
        sf::CircleShape dot(6);
        dot.setPosition(SIDE_X + 10, y + 7);
        dot.setFillColor(colorFor(*p));
        target.draw(dot);
        drawText(target, std::to_string(p->id) + "  " + p->name, SIDE_X + 30, y + 4, 14, p->alive() ? TEXT : DIM);
        drawText(target, p->playerClass == "ARCHER" ? "ARC" : "WAR", SIDE_X + 176, y + 5, 12, DIM);
        drawHpBar(target, SIDE_X + 214, y + 10, 70, 6, p->hp);
        drawText(target, std::to_string(p->score), SIDE_X + SIDE_W - 28, y + 4, 14, ACCENT, true);
        y += 28;
    }
    y = std::max(y + 14, BOARD_Y + 420);

    // Journal
    drawText(target, "EVENTS", SIDE_X, y, 13, DIM, true);
    y += 22;
    const auto& log = _state.log();
    const std::size_t lines = 7;
    std::size_t start = log.size() > lines ? log.size() - lines : 0;
    for (std::size_t i = start; i < log.size(); ++i) {
        const std::string& entry = log[i];
        sf::Color color = entry.rfind("✖", 0) == 0 ? DANGER : entry.rfind("☠", 0) == 0 ? ACCENT : TEXT;
        std::string clipped = entry.size() > 46 ? entry.substr(0, 45) + "…" : entry;
        drawText(target, clipped, SIDE_X, y, 13, color);
        y += 20;
    }

    // Aide
    drawText(target, "Move: arrows / WASD / ZQSD   Attack: click or Space", SIDE_X, HEIGHT - 48, 12, DIM);
    drawText(target, "L: refresh   Esc: logout", SIDE_X, HEIGHT - 30, 12, DIM);
}
