/*
** EPITECH PROJECT, 2026
** teck_reseau_arena
** File description:
** Server
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <memory>
#include "./ClientSession.hpp"
namespace net {
    class Server {
        public:
            Server();
            ~Server();

        protected:
        private:
            std::map<net::ClientAddress, std::unique_ptr<net::ClientSession>> _sessions;
    };
}

#endif /* !SERVER_HPP_ */
