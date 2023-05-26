/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:43:40 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/26 18:01:35 by jbettini         ###   ########.fr       */
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
                    std::cout << newUser.getNick() << " new USER" << std::endl;
                    std::cout << this->getClient(newClient).getNick() << " new USER IN CLIENT LIST" << std::endl;
                    // add_back(this->_ClientList, newUser);
                    this->getClient(newClient).setNick("*"); // Mystere incomprehensible
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
                    std::cout << ClientFd[i].fd << "ClientSocket" << std::endl;
                    this->execInput(splitBuffer(buffer, " \v\n\t\r\f"),  (this->getClient(ClientFd[i].fd)));
                }
            }
        }
    }
}


void    server::modeFun(Client & client, std::vector<std::string> clientInput) {
    std::string mode = clientInput[clientInput.size() - 1];
    if (mode == "+i"){
        std::string tmp = ":127.0.0.1 221 " + client.getNick() + " :+i\r\n";
        this->displayClient(tmp, client);
    }
}

void    server::pingFun(Client & client, std::vector<std::string> clientInput) {
    (void)clientInput;
    this->displayClient("PONG :127.0.0.1\r\n", client);
}

// Faire la taille max = 9
void    server::nickFun(Client & client, std::vector<std::string> clientInput) {
    std::vector<Client>::iterator it;
    std::cout << client.getCS() << ": THE CLIENT " << std::endl;
    for (it = this->_ClientList.begin(); it != this->_ClientList.end(); it++) {
        std::cout << (*it).getNick() << " = nick : client "  << (*it).getCS() << std::endl;
        if ((*it).getNick() ==  clientInput[1]) {
            this->displayClient(":127.0.0.1 433 " + client.getNick() + " " + clientInput[1] + " :Nickname is already in use.\r\n", client);
            return;
        }
    }
    client.setNick(clientInput[1]);
    if (client.isSetup() && client.getWelcome() == 0)
        this->welcomeMsg(client);
}
                                                                                                                                                                                                                                                                                                          
void    server::userFun(Client & client, std::vector<std::string> clientInput) {
    client.setUsername(clientInput[1]);
    if (client.isSetup() && client.getWelcome() == 0) {
        this->welcomeMsg(client);
    }
}

void    server::passFun(Client & client, std::vector<std::string> clientInput)  {
    if (client.getPass() == 1)
        this->displayClient(":127.0.0.1 462 " + client.getNick() + " :You may not reregister.\r\n", client);
    else if (clientInput[1] == this->_password)
        client.setPass(1);
    else
        this->displayClient(":127.0.0.1 464 " + client.getNick() + " :Incorrect Password.\r\n", client);
    if (client.isSetup() && client.getWelcome() == 0) {
        this->welcomeMsg(client);
    }
}

void        server::welcomeMsg(Client & client){
    this->displayClient(":127.0.0.1 001 " +  client.getNick() + " :Vous êtes connecté avec succès à mon serveur\r\n", client);   
    client.setWelcome();
}

void        server::capFun(Client & client, std::vector<std::string> clientInput) {
    std::vector<std::string>    tmp = makeVecKey(clientInput, "PASS");
    if (tmp != clientInput)
        (this->*_FunLst[tmp[0]])(client, tmp);
    tmp = makeVecKey(clientInput, "USER");
    if (tmp != clientInput)
        (this->*_FunLst[tmp[0]])(client, tmp);
    tmp = makeVecKey(clientInput, "NICK");
    if (tmp != clientInput)
        (this->*_FunLst[tmp[0]])(client, tmp);
}

void    server::initFunLst(void)
{
    this->_FunLst["MODE"] = &server::modeFun;
    this->_FunLst["PING"] = &server::pingFun;
    this->_FunLst["NICK"] = &server::nickFun;
    this->_FunLst["USER"] = &server::userFun;
    this->_FunLst["PASS"] = &server::passFun;
    this->_FunLst["CAP"] =  &server::capFun;
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
    send(client.getCS(), msg.c_str(), msg.size(), 0);
}



void        server::execInput(std::vector<std::string> clientInput, Client & client) {
        Fun fun = _FunLst[clientInput[0]];
        if (fun) {
            (this->*fun)(client, clientInput);
        }
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

//REMOVE CHANNEL USER WHEN DC /WARNING

void        server::disconnectClient(Client & client) {
    this->_ClientFd[client.getPollFd()].fd = 0;
    this->_ClientFd[client.getPollFd()].events = 0;
    this->_ClientFd[client.getPollFd()].revents = 0;
    // pthread_detach(client.getThread());
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

void printClient(std::vector<Client> strings) {
    for (size_t i = 0; i < strings.size(); ++i) {
        std::cout << "ClientSocket : " << strings[i].getCS()<< std::endl;
        std::cout << "ClientUsername : " << strings[i].getUsername() << std::endl;
        std::cout << "ClientNick : " << "\"" << strings[i].getNick() << "\"" <<std::endl;
    }
}

int findString(std::vector<std::string> strings, std::string toFind) {
    for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end();it++)
        if (*it == toFind)
            return (1);
    return (0);
}

std::string retAfterFind(std::vector<std::string> strings, std::string toFind) {
    for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end();it++)
        if (*it == toFind)
            return (*(++it));
      
    return (toFind);
}


std::vector<std::string> makeVecKey(std::vector<std::string> strings, std::string toFind) {
    if (findString(strings, toFind)) {
        std::vector<std::string>    ret;
        ret.push_back(toFind);
        std::string after = retAfterFind(strings, toFind);
        if (after != toFind)
            ret.push_back(after);
        else
            return strings;
        return ret;
    }
    return strings;
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

void add_back(std::vector<Client> & vector, const Client & objet) {
    Client  copie(objet);
    vector.resize(vector.size() + 1); 
    vector[vector.size()] = copie;
}

std::vector<std::string> splitBuffer(char* buffer, const std::string& delimiters) {
    std::vector<std::string> splited;
    std::string tmp(buffer);
    size_t pos = 0;
    
    while ((pos = tmp.find_first_of(delimiters)) != std::string::npos) {
        std::string token = tmp.substr(0, pos);
        tmp.erase(0, pos + 1);
        splited.push_back(token);
    }
    
    splited.push_back(tmp);
    splited = removeWhitespace(splited);
    return splited;
}
