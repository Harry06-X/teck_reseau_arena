/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** AccountManager
*/

#include "auth/AccountManager.hpp"
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
    std::string toHex(const unsigned char *data, std::size_t len)
    {
        std::ostringstream out;
        for (std::size_t i = 0; i < len; ++i)
            out << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        return out.str();
    }
}

AccountManager::AccountManager(std::string storagePath) : _storagePath(std::move(storagePath))
{
    load();
}

AccountManager::Result AccountManager::registerUser(const std::string& username, const std::string& password)
{
    if (_accounts.count(username))
        return Result::AlreadyExists;

    Account account;
    account.salt = randomSalt();
    account.hash = hashPassword(password, account.salt);
    _accounts[username] = account;
    save(username, account);
    return Result::Ok;
}

AccountManager::Result AccountManager::authenticate(const std::string& username,
    const std::string& password, uint32_t ip)
{
    if (isRateLimited(ip))
        return Result::TooManyAttempts;

    auto it = _accounts.find(username);
    Result result = Result::Ok;
    if (it == _accounts.end()) {
        result = Result::UnknownUser;
    } else {
        std::string candidate = hashPassword(password, it->second.salt);
        // Comparaison en temps constant (pas de fuite d'information par le timing)
        if (candidate.size() != it->second.hash.size()
            || CRYPTO_memcmp(candidate.data(), it->second.hash.data(), candidate.size()) != 0)
            result = Result::WrongPassword;
    }

    if (result != Result::Ok)
        _failures[ip].push_back(Clock::now());
    else
        _failures.erase(ip);
    return result;
}

bool AccountManager::isRateLimited(uint32_t ip)
{
    auto it = _failures.find(ip);
    if (it == _failures.end())
        return false;
    const auto now = Clock::now();
    auto& attempts = it->second;
    while (!attempts.empty() && now - attempts.front() > FAILURE_WINDOW)
        attempts.pop_front();
    return static_cast<int>(attempts.size()) >= MAX_FAILURES;
}

std::string AccountManager::randomSalt()
{
    unsigned char salt[16];
    if (RAND_bytes(salt, sizeof(salt)) != 1)
        throw std::runtime_error("RAND_bytes failed");
    return toHex(salt, sizeof(salt));
}

std::string AccountManager::hashPassword(const std::string& password, const std::string& salt)
{
    unsigned char out[32];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.size()),
            reinterpret_cast<const unsigned char *>(salt.data()), static_cast<int>(salt.size()),
            10000, EVP_sha256(), sizeof(out), out) != 1)
        throw std::runtime_error("PBKDF2 failed");
    return toHex(out, sizeof(out));
}

void AccountManager::load()
{
    std::ifstream file(_storagePath);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream in(line);
        std::string username;
        Account account;
        if (std::getline(in, username, ':') && std::getline(in, account.salt, ':') && std::getline(in, account.hash))
            _accounts[username] = account;
    }
}

void AccountManager::save(const std::string& username, const Account& account) const
{
    std::ofstream file(_storagePath, std::ios::app);
    if (!file)
        throw std::runtime_error("Cannot write accounts file " + _storagePath);
    file << username << ':' << account.salt << ':' << account.hash << '\n';
}
