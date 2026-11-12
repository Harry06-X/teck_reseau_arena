/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** ProtocolParser — format officiel MSGID|TYPE|FIELD1|FIELD2|...
*/

#ifndef PROTOCOLPARSER_HPP_
#define PROTOCOLPARSER_HPP_

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace net {

    // Levée pour tout datagramme mal formé ou champ invalide
    class ProtocolError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    struct ParsedMessage {
        uint32_t msgId = 0;               // 0 = message sans ACK attendu (ACK, GAME_UPDATE)
        std::string type;                 // MOVE, LOGIN, STATE...
        std::vector<std::string> fields;  // FIELD1, FIELD2...
    };

    class ProtocolParser {
    public:
        static constexpr char SEPARATOR = '|';
        static constexpr std::size_t MAX_DATAGRAM = 1400;
        static constexpr std::size_t MAX_FIELD = 64;
        static constexpr std::size_t MAX_PAYLOAD_FIELD = 1200; // GAME_UPDATE

        // "42|MOVE|UP" → { 42, "MOVE", {"UP"} } — lève ProtocolError
        static ParsedMessage parse(const std::string& raw);

        // { 42, "STATE", {"100","3","4","0"} } → "42|STATE|100|3|4|0"
        // Les caractères interdits ('|', contrôle) sont neutralisés par sécurité.
        static std::string serialize(uint32_t msgId, const std::string& type,
            const std::vector<std::string>& fields = {});

        // Nom d'utilisateur : 3 à 16 caractères [A-Za-z0-9_]
        static bool isValidUsername(const std::string& name);
        // Mot de passe : 4 à 32 caractères imprimables, sans '|'
        static bool isValidPassword(const std::string& password);

    private:
        static std::vector<std::string> split(const std::string& s, char delimiter);
        static bool isPrintable(const std::string& s);
    };
}

#endif /* !PROTOCOLPARSER_HPP_ */
