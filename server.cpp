/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:43:40 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/23 23:07:40 by jbettini         ###   ########.fr       */
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
    struct  pollfd      ClientFd[MAX_CLIENTS + 1];
    
    this->init_socket();
    this->initFunLst();
    // Initialisation du tableau des descripteurs de fichiers pour la fonction poll
    memset(ClientFd, 0, sizeof(ClientFd));
    ClientFd[0].fd = this->_socket;
    ClientFd[0].events = POLLIN;
    this->_ClientFd = ClientFd;
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
              // Ajout du nouveau Client à la liste
            for (int i = 1; i < this->MAX_CLIENTS + 1; i++)
                if (ClientFd[i].fd == 0) {
                    ClientFd[i].fd = newClient;
                    ClientFd[i].events = POLLIN;

                    std::string welcomeMsg = ":127.0.0.1 001 ffiliz :Vous êtes connecté avec succès à mon serveur\r\n";
                    send(ClientFd[i].fd, welcomeMsg.c_str(), welcomeMsg.size(), 0);
                    Client newUser(newClient, i);
                    this->_ClientList.push_back(newUser); 
                    break;
                }
        }
        // Lecture et traitement des données des Clients connectés
        for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
            if (ClientFd[i].fd > 0 && ClientFd[i].revents & POLLIN) {
                size_t bytesRead = recv(ClientFd[i].fd, buffer, this->MAX_BUFFER_SIZE, 0);
                buffer[bytesRead] = '\0';
                //if (strncmp("MODE", buffer, 4) != 0)
                //{
                    if (buffer[0] == 10 && buffer[1] == '\0')
                        break;
                    if (bytesRead < 0)
                        throw recvException();
                    else if (bytesRead == 0)
                        this->disconnectClient(this->getClient(ClientFd[i].fd));
                    else {
                        // this->parseInput(splitBuffer(buffer, ' '),  (this->getClient(ClientFd[i].fd)));
                        if (strncmp("MODE", buffer, 4) == 0)
                            send(ClientFd[i].fd, ":127.0.0.1 221 ffiliz :+i\r\n", 28, 0);
                        if (strncmp("PING", buffer, 4) == 0)
                        {
                            std::cout << "0 " << buffer << std::endl;
                            send(ClientFd[i].fd, "PONG :127.0.0.1\r\n", 18, 0);
                        }
                    }
                //}
            }
        }

    }
}

void    server::initFunLst(void)
{
    this->_FunLst["/nick"] = &server::defineClientUsername;
    // this->_FunLst["/list"] = &server::;
    // this->_FunLst["/join"] = &server::;
    // this->_FunLst["/part"] = &server::;
    // this->_FunLst["/msg"] = &server::;
    // this->_FunLst["/kick"] = &server::;
    // this->_FunLst["/ban"] = &server::;
    // this->_FunLst["/unban"] = &server::;
    // this->_FunLst["/exit"] = &server::;
    // this->_FunLst["/op"] = &server::;
    // this->_FunLst["/deop"] = &server::;
    // this->_FunLst["/silence"] = &server::;
    // this->_FunLst["/unsilence"] = &server::;
}

//  Server Funct

void    server::displayClient(std::string   msg, Client client) {
    // need to send to irssi and nc 
    send(client.getCS(), msg.c_str(), msg.size(), 0);

}

void        server::defineClientUsername(Client & client, std::vector<std::string> clientInput) {
    std::vector<Client>::iterator it;
    for (it = this->_ClientList.begin(); it != this->_ClientList.end(); it++) {

        if ((*it).getUsername() ==  clientInput[1]) {
            this->displayClient("Error: this username isn't available\n", client);
            return;
        }
    }
    client.setName(clientInput[1]);
}



void        server::parseInput(std::vector<std::string> clientInput, Client & client) {
    
    if (client.getUsername() == "anonyme" && clientInput[0] != "/nick") {
        this->displayClient("Error: you need to set a nick with \"/nick MonPseudo modifiera votre pseudonyme en MonPseudo\"\n", client);
        return ;
    }
}

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
//REMOVE CHANNEL USER WHEN DC /WARNING

void        server::disconnectClient(Client & client) {
    this->_ClientFd[client.getPollFd()].fd = 0;
    this->_ClientFd[client.getPollFd()].events = 0;
    this->_ClientFd[client.getPollFd()].revents = 0;
    close(client.getCS());
    pthread_detach(client.getThread());
    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
        if (it->getCS() == client.getCS()) {
            this->_ClientList.erase(it);
            break;
        }
    std::cout << "Client disconnect !" << std::endl;
}

void        server::exitClient(Client & client, std::vector<std::string> clientInput) {
    (void)clientInput;
    this->_ClientFd[client.getPollFd()].fd = 0;
    this->_ClientFd[client.getPollFd()].events = 0;
    this->_ClientFd[client.getPollFd()].revents = 0;
    close(client.getCS());
    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
        if (it->getCS() == client.getCS()) {
            this->_ClientList.erase(it);
            break;
        }
    std::cout << "Client disconnect !" << std::endl;
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

void printVecStr(std::vector<std::string> strings) {
    for (size_t i = 0; i < strings.size(); ++i)
        std::cout << "\" str = " + strings[i] + "\""<< std::endl;
}

std::vector<std::string> removeWhitespace(std::vector<std::string>& strings) {
    for (size_t i = 0; i < strings.size(); ++i) {
        std::string& str = strings[i];
        std::string::iterator iter = str.begin();
        while (iter != str.end()) {
            if (*iter == ' ' || (*iter >= 9 && *iter <= 13)) {
                iter = str.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    return strings;
}

std::vector<std::string>    splitBuffer(char *buffer, char delimiter){

    std::vector<std::string>    splited;
    std::stringstream           ss(buffer);
    std::string                 tmp;

    while (std::getline(ss, tmp, delimiter)) {
        splited.push_back(tmp);
    }
    splited = removeWhitespace(splited);
    return splited;
}
