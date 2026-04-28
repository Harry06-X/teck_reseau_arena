#include "../../include/network/Server.hpp"

// ecs::ComponentManager compManager;

// // 1. Enregistrer les types au démarrage
// compManager.registerComponent<PositionComponent>();

// // 2. Ajouter un composant à une entité
// compManager.addComponent(myEntityId, PositionComponent{10, 20});

// // 3. Récupérer la donnée
// auto& pos = compManager.getComponent<PositionComponent>(myEntityId);

#include "../include/network/UdpSocket.hpp"
#include "../include/network/ProtocolParser.hpp"
#include <iostream>

#include <map>
#include <iostream>
#include <algorithm>
#include "network/UdpSocket.hpp"
#include "network/ClientSession.hpp"
#include "ecs/EntityManager.hpp" // Pour créer des IDs d'entités

int main() {
    net::UdpSocket server;
    ecs::EntityManager entityManager; // Géré par ton binôme normalement
    
    // Le registre des sessions : Clé = IP/Port, Valeur = Session
    std::map<net::ClientAddress, std::unique_ptr<net::ClientSession>> sessions;

    if (!server.bind(4242)) return 84;

    std::cout << "Serveur actif. Surveillance des sessions (Timeout: 60s)..." << std::endl;

    char buffer[1024];
    struct sockaddr_in clientAddr;

    while (true) {
        std::memset(buffer, 0, 1024);
        ssize_t bytes = server.receiveFrom(buffer, 1024, clientAddr);
        
        if (bytes > 0) {
            // 1. On crée la clé unique pour ce client
            net::ClientAddress addr = { clientAddr.sin_addr.s_addr, clientAddr.sin_port };

            // 2. On vérifie si c'est un nouveau ou un ancien
            if (sessions.find(addr) == sessions.end()) {
                std::cout << "Nouveau joueur détecté ! Création de la session..." << std::endl;
                
                // On crée une entité pour ce nouveau joueur
                ecs::Entity newPlayer = entityManager.createEntity();
                sessions[addr] = std::make_unique<net::ClientSession>(newPlayer);
            } else {
                // Joueur connu : on met à jour son chrono d'activité
                sessions[addr]->updateLastActivity();
            }

            std::cout << "Message de Joueur [Entity " << sessions[addr]->getEntityId() << "]: " << buffer << std::endl;
        }

        // 3. Nettoyage des sessions inactives (Timeout)
        // On utilise un itérateur pour supprimer proprement pendant le parcours
        for (auto it = sessions.begin(); it != sessions.end(); ) {
            if (it->second->isTimedOut(60.0)) {
                std::cout << "Timeout : Le joueur [Entity " << it->second->getEntityId() << "] a été déconnecté." << std::endl;
                // Ici, il faudra aussi détruire l'entité dans l'ECS via l'EntityManager
                it = sessions.erase(it); 
            } else {
                ++it;
            }
        }
    }
    return 0;
}