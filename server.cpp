/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:43:40 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/18 21:26:20 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

// Server

server::server(void) :  MAX_CLIENTS(10) ,MAX_BUFFER_SIZE(4097), _port(6667), _password("4242")   {}

server::server(int port, std::string pswd) :   MAX_CLIENTS(10) ,MAX_BUFFER_SIZE(4097), _port(port), _password(pswd) {
}

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
    struct  pollfd      ClientFd[MAX_CLIENTS];
    
    this->init_socket();    
    // Initialisation du tableau des descripteurs de fichiers pour la fonction poll
    memset(ClientFd, 0, sizeof(ClientFd));
    ClientFd[0].fd = this->_socket;
    ClientFd[0].events = POLLIN;
    std::cout << "Server Irc Operationel !" << std::endl;
    while(true) {
        // Attente des événements
        int event = poll(ClientFd, this->MAX_CLIENTS + 1, -1);
        if (event < 0)
            throw pollException();
        // / Vérification des nouvelles connexions
        if (ClientFd[0].revents & POLLIN) {
            if ((newClient = accept(this->_socket, 0, 0)) < 0)
                throw acceptException();
            else
                std::cout << "new Client" << std::endl;
            std::string welcomeMsg = "Welcome to Our Server IRC\n";;
            send(newClient, welcomeMsg.c_str(), welcomeMsg.size(), 0);
              // Ajout du nouveau Client à la liste
            for (int i = 1; i < this->MAX_CLIENTS + 1; i++)
                if (ClientFd[i].fd == 0) {
                    ClientFd[i].fd = newClient;
                    ClientFd[i].events = POLLIN;
                    Client newUser(newClient, i);
                    this->_ClientList.push_back(newUser); 
                    break;
                }
        }
        // Lecture et traitement des données des Clients connectés
        for (int i = 1; i < MAX_CLIENTS + 1; ++i)
            if (ClientFd[i].fd > 0 && ClientFd[i].revents & POLLIN) {
                size_t bytesRead = recv(ClientFd[i].fd, buffer, this->MAX_BUFFER_SIZE, 0);
                    buffer[bytesRead] = '\0';
                    if (bytesRead < 0)
                        throw recvException();
                    else if (bytesRead == 0)
                        this->disconnectClient(ClientFd[i]);
                    else
                        this->parseInput(splitBuffer(buffer, ' '), this->getClient(newClient));
            }
    }
}

//  Server Funct


void    server::init_socket(void) {

    // Création du socket
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1)
        throw socketException();

    // Parametrage du socket
    int reuse = 1;
    if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        throw socketException();
    
    // Configuration de l'adresse serveur
    this->_serverAddress.sin_family = AF_INET;
    this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
    this->_serverAddress.sin_port = htons(this->_port);

    // Attachement du socket à l'adresse et au port
    if (bind(this->_socket, reinterpret_cast<struct sockaddr*>(&(this->_serverAddress)), sizeof(this->_serverAddress)) < 0)
        throw bindException();
    
    // Écoute des connexions entrantes
    if (listen(this->_socket, this->MAX_CLIENTS) < 0)
        throw listenException();
}

void        server::disconnectClient(struct pollfd & ClientFd) {
    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
        if (it->getCS() == ClientFd.fd) {
            this->_ClientList.erase(it);
            break;
        }
    close(ClientFd.fd);
    ClientFd.fd = 0;
    ClientFd.events = 0;
    ClientFd.revents = 0;
    std::cout << "Client disconnect !" << std::endl;
}

void        server::parseInput(std::vector<std::string> clientInput, Client client) {
    if (client.getUsername() == "anonyme" && clientInput[0] != "/nick")


}

// Channel function

void    server::addChannel(std::string  name) {
    Channel newChannel(name);
    this->_ChannelList.push_back(newChannel);
}

void    server::printChannel(void) {
        std::cout << "Channel List : " << std::endl;
    for (std::vector<Channel>::iterator it = _ChannelList.begin(); it != _ChannelList.end(); it++)
        std::cout << "#" << it->getName() << std::endl;
}
// Utils

std::vector<std::string>    splitBuffer(char *buffer, char delimiter){

    std::vector<std::string>    splited;
    std::stringstream           ss(buffer);
    std::string                 tmp;

    while (std::getline(ss, tmp, delimiter))
        splited.push_back(tmp);
    return splited;
}