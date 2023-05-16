/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:43:40 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/16 21:32:02 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"


server::server(void) :  MAX_CLIENTS(10) ,MAX_BUFFER_SIZE(4097), _port(6667), _password("4242")   {}

server::server(int port, std::string pswd) :   MAX_CLIENTS(10) ,MAX_BUFFER_SIZE(4097), _port(port), _password(pswd) {}

server &    server::operator=(server & rhs) {
    this->_port = rhs.getPort();
    this->_password = rhs.getPassword();
    return *this;
}

server::server(server & rhs) {
    *this = rhs;
}

server::~server(void) {
    close(this->_socket);
};

void    server::run(void) {
    int     newClient;
    char    buffer[this->MAX_BUFFER_SIZE];
    struct  sockaddr_in serverAddress = {};
    struct  pollfd clientFd[this->MAX_CLIENTS];

    // Création du socket
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1)
        throw socketException();

    // Parametrage du socket
    int reuse = 1;
        if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        throw socketException();

    // Configuration de l'adresse serveur
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(this->_port);
    
    // Attachement du socket à l'adresse et au port
    if (bind(this->_socket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0)
        throw bindException();

    // Écoute des connexions entrantes
    if (listen(this->_socket, this->MAX_CLIENTS) < 0)
        throw listenException();

    // Initialisation du tableau des descripteurs de fichiers pour la fonction poll
    memset(clientFd, 0, sizeof(clientFd));
    clientFd[0].fd = this->_socket;
    clientFd[0].events = POLLIN;
    std::cout << "Server Irc Operationel !" << std::endl;
    while(true) {
        // Attente des événements
        int event = poll(clientFd, this->MAX_CLIENTS + 1, -1);
        if (event < 0)
            throw pollException();
        
        // / Vérification des nouvelles connexions
        if (clientFd[0].revents & POLLIN) {
            if ((newClient = accept(this->_socket, 0, 0)) < 0)
                throw acceptException();
            else
                std::cout << "new client" << std::endl;
            std::string welcomeMsg = "Welcome to Our Server IRC";;
            send(newClient, welcomeMsg.c_str(), welcomeMsg.size(), 0);
        }
        // Ajout du nouveau client à la liste
        for (int i = 1; i < this->MAX_CLIENTS + 1; i++)
            if (clientFd[i].fd == 0) {
                clientFd[i].fd = newClient;
                clientFd[i].events = POLLIN;
                // connectedClients.push_back(i);
                break;
            }
        // Lecture et traitement des données des clients connectés
        for (int i = 1; i < MAX_CLIENTS + 1; ++i)
            if (clientFd[i].fd > 0 && clientFd[i].revents & POLLIN) {
                size_t bytesRead = recv(clientFd[i].fd, buffer, this->MAX_BUFFER_SIZE, 0);
                    buffer[bytesRead] = '\0';
                    if (bytesRead < 0)
                        throw recvException();
                    else if (bytesRead == 0) {
                        close(clientFd[i].fd);
                        std::cout << "client disconnect !" << std::endl;
                    }
                    else {
                        std::cout << "msg recu !" << std::endl;
                        std::cout << buffer << std::endl;
                    }
            }
    }
}
