/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:43:40 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/28 21:13:03 by jbettini         ###   ########.fr       */
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
            {
                if (ClientFd[i].fd == 0) {
                    ClientFd[i].fd = newClient;
                    ClientFd[i].events = POLLIN;
                    
                    Client newUser(newClient, i);
                    this->_ClientList.push_back(newUser);
                    break;
                }
            }
        }
        // Lecture et traitement des données des Clients connectés
        for (int i = 1; i < MAX_CLIENTS + 1; i++) {
            if (ClientFd[i].fd > 0 && ClientFd[i].revents & POLLIN) {
                size_t bytesRead = recv(ClientFd[i].fd, buffer, this->MAX_BUFFER_SIZE, 0);
                buffer[bytesRead] = '\0';

                if (buffer[0] == 10 && buffer[1] == '\0')
                    break;
                if (bytesRead < 0)
                    throw recvException();
                else if (bytesRead == 0)
                    this->disconnectClient(this->getClient(ClientFd[i].fd));
                else {
                    std::cout << buffer << std::endl;
                    // std::cout << ClientFd[i].fd << " ClientSocket" << std::endl;
                    this->execInput(splitBuffer(buffer, " \v\n\t\r\f"),  (this->getClient(ClientFd[i].fd)));
                }
            }
        }
    }
}

// void    server::sendToAllClientChannel(Client & client, std::vector<std::string> clientInput, int check)
// {
//     std::cout << " dans quit INPUT DE 0 ->" <<  clientInput[0] << "<-" << std::endl;
//     if (check == 0) {
//         for(std::vector<std::string>::iterator it = client.getAllChannel().begin(); it != client.getAllChannel().end(); it++){
//             this->sendToAllUserInChannel(*it, ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip QUIT :Quit: " + client.getNick() + "\r\n", client);
//         }
//     }
//     else {
//         for(std::vector<std::string>::iterator it = client.getAllChannel().begin(); it != client.getAllChannel().end(); it++){
//             this->sendToAllUserInChannel(*it, ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip QUIT :Quit: " + clientInput[1], client);
//         }
//     }
//     disconnectClient(client);
// }

// void    server::quitFun(Client & client, std::vector<std::string> clientInput) {
//     if (clientInput.size() - 1 == 0)
//         sendToAllClientChannel(client, clientInput, 0);
//     else {
//         sendToAllClientChannel(client, clientInput, 1);
//     }
// }

// :xtem!~xtem@6be1-f476-da9-512d-d573.rev.sfr.net MODE #4242 +b *!*jbe@*.rev.sfr.net


//  Server Funct

std::string    server::getAllUsersChannel(Channel & channel) {
    std::string userList;
    std::string tmp;
    for(std::vector<Client>::const_iterator it = channel.getChannelUser().begin(); it != channel.getChannelUser().end(); it++) {
        Client tmp2 = *it;
        if (channel.isOp(tmp2.getNick()))
            tmp = "@" + tmp2.getNick();
        else
            tmp = tmp2.getNick();
        userList += tmp;
        if (it + 1 != channel.getChannelUser().end())
            userList += " ";
    }
    return userList;
}

void        server::execInput(std::vector<std::string> clientInput, Client & client) {
    //printVecStr(clientInput);
    Fun fun = _FunLst[clientInput[0]];
    if (fun && client.getWelcome()) 
        (this->*fun)(client, clientInput);
    else if (fun && checkFunWelcome(clientInput[0]) && !client.getWelcome())
        (this->*fun)(client, clientInput);
    else if (fun && !checkFunWelcome(clientInput[0]) && !client.getWelcome())
        this->displayClient(":127.0.0.1 421 " + client.getNick() + " " + clientInput[0] + " :You need to be connected to Server, Please set Nick, User and Pass.\r\n", client);
    else
        this->displayClient(":127.0.0.1 421 " + client.getNick() + " " + clientInput[0] + " :Unknow command\r\n", client);
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


void        server::disconnectClient(Client & client) {
    this->removeUserInChannel(client);
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

void        server::removeUserInChannel(Client & client) {
    for (std::vector<std::string>::iterator it = client.getAllChannel().begin(); it != client.getAllChannel().end(); it++) {
        for (std::vector<Channel>::iterator it2 = this->_ChannelList.begin(); it2 != this->_ChannelList.end(); it2++) {
            if (it2->getChannelName() == *it) {
                it2->removeUser(client);
                if (it2->getChannelUser().size() == 0) {
                    this->_ChannelList.erase(it2);
                }
            }
        }
    }
}

// Quand un user change de nick et qu'il fait parti d'un channel mettre a . jour tout les vecteur des channel ou il se trouve

void        server::updateChannelListNick(std::vector<Channel>  & channelList, std::string currentNick, std::string newNick) {
    for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); it++) {
        for (std::vector<Client>::iterator it2 = it->getChannelUser().begin(); it2 != it->getChannelUser().end(); it2++) {
            if (it2->getNick() == currentNick) {
                it2->setNick(newNick);
                updateVectorStr(it->getSilentList(), currentNick, newNick);
                updateVectorStr(it->getBannedList(), currentNick, newNick);
                updateVectorStr(it->getOpUserList(), currentNick, newNick);
            }
        }
    }
}