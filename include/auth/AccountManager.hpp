/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AccountManager — comptes joueurs (REGISTER / LOGIN)
*/

#ifndef ACCOUNTMANAGER_HPP_
#define ACCOUNTMANAGER_HPP_

#include <chrono>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>

/*
 * Les mots de passe ne sont jamais stockés en clair :
 * PBKDF2-HMAC-SHA256 (10 000 itérations) avec un sel aléatoire de 16 octets.
 * Les comptes sont persistés dans un fichier texte "username:sel:hash".
 * Anti brute-force : 5 échecs de connexion max par IP et par minute.
 */
class AccountManager {
public:
    enum class Result {
        Ok,
        AlreadyExists,
        UnknownUser,
        WrongPassword,
        TooManyAttempts
    };

    static constexpr int MAX_FAILURES = 5;
    static constexpr std::chrono::seconds FAILURE_WINDOW{60};

    explicit AccountManager(std::string storagePath);

    Result registerUser(const std::string& username, const std::string& password);
    Result authenticate(const std::string& username, const std::string& password, uint32_t ip);

private:
    using Clock = std::chrono::steady_clock;

    struct Account {
        std::string salt; // hex
        std::string hash; // hex
    };

    static std::string randomSalt();
    static std::string hashPassword(const std::string& password, const std::string& salt);

    void load();
    void save(const std::string& username, const Account& account) const;
    bool isRateLimited(uint32_t ip);

    std::string _storagePath;
    std::unordered_map<std::string, Account> _accounts;
    std::unordered_map<uint32_t, std::deque<Clock::time_point>> _failures;
};

#endif /* !ACCOUNTMANAGER_HPP_ */
