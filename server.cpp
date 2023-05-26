/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:43:40 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/26 23:09:28 by jbettini         ###   ########.fr       */
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

void    server::sendToAllClientChannel(Client & client, std::vector<std::string> clientInput, int check)
{
    std::cout << " dans quit INPUT DE 0 ->" <<  clientInput[0] << "<-" << std::endl;
    if (check == 0) {
        for(std::vector<std::string>::iterator it = client.getAllChannel().begin(); it != client.getAllChannel().end(); it++){
            this->sendToAllUserInChannel(*it, ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip QUIT :Quit: " + client.getNick() + "\r\n", client);
        }
    }
    else {
        for(std::vector<std::string>::iterator it = client.getAllChannel().begin(); it != client.getAllChannel().end(); it++){
            this->sendToAllUserInChannel(*it, ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip QUIT :Quit: " + clientInput[1], client);
        }
    }
    disconnectClient(client);
}

void    server::quitFun(Client & client, std::vector<std::string> clientInput) {
    if (clientInput.size() - 1 == 0)
        sendToAllClientChannel(client, clientInput, 0);
    else {
        sendToAllClientChannel(client, clientInput, 1);
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

void    server::nickFun(Client & client, std::vector<std::string> clientInput) {
    std::vector<Client>::iterator it;
    const std::string nick = std::string(clientInput[1]);

    if (nick.size() > 9)
    {
        this->displayClient(":127.0.0.1 432 " + client.getNick() + " " + nick + " :Nickname is too long (>9).\r\n", client);
        return;
    }
    else if (checkNonAlphanumeric(nick)) {
        this->displayClient(":127.0.0.1 432 " + client.getNick() + " " + nick + " :Nickname contain undefine caracter.\r\n", client);
        return ;
    }
    for (it = this->_ClientList.begin(); it != this->_ClientList.end(); it++) {
        if ((*it).getNick() == nick) {
            this->displayClient(":127.0.0.1 433 " + client.getNick() + " " + nick + " :Nickname is already in use.\r\n", client);
            return;
        }
    }

    client.setNick(nick);
    //Complete connexion.
    if (client.isSetup() && client.getWelcome() == 0)
        this->welcomeMsg(client);
}
                                                                                                                                                                                                                                                                                                          
void    server::userFun(Client & client, std::vector<std::string> clientInput) {
    client.setUsername(clientInput[1]);

    //Complete connexion.
    if (client.isSetup() && client.getWelcome() == 0) {
        this->welcomeMsg(client);
    }
}

void    server::passFun(Client & client, std::vector<std::string> clientInput)  {
    const std::string pass = std::string(clientInput[1]);
    
    if (client.getPass() == 1)
        this->displayClient(":127.0.0.1 462 " + client.getNick() + " :You may not reregister.\r\n", client);
    else if (pass == this->_password)
        client.setPass(1);
    else if (client.getNick() == "admin" && pass == "admin")
    {
        client.setPass(1);
        client.setOp(true);
    }
    else
        this->displayClient(":127.0.0.1 464 " + client.getNick() + " :Incorrect Password.\r\n", client);
    
    //Complete connexion.
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

// void    server::sendChannelMessage(Client & client, std::vector<std::string> clientInput)
// {
//     for (std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++)
//     {
//         if ((it)->getChannelName() == client.getChannel())
//         {
//             std::string res;
//             for (std::vector<std::string>::const_iterator it = clientInput.begin(); it != clientInput.end(); ++it)
//                 res += *it + ' ';
//             (it)->sendMessage(client, res);
//             return;
//         }
//     }


void    server::sendToAllUserInChannel(std::string channelName, std::string msg, Client & client) {
    for(std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++) {
        if ((*it).getChannelName() == channelName) {
            for (std::vector<Client>::iterator it2 = (*it).getChannelUser().begin(); it2 != (*it).getChannelUser().end(); it2++) {
                if ((*it2).getNick() != client.getNick())
                    this->displayClient(msg, *it2);
            }
        }
    }
}

std::string    server::getAllUsersChannel(Channel channel) {
    std::string userList;
    std::string tmp;
    for(std::vector<Client>::iterator it = channel.getChannelUser().begin(); it != channel.getChannelUser().end(); it++) {
        if (channel.isOp(*it))
            tmp = "@" + (*it).getNick();
        else
            tmp = (*it).getNick();
        userList += tmp;
        userList += " ";
    }
    return userList;
}

void    server::welcomeToChannel(Client & client, std::string channelName) {

    this->displayClient(":" + client.getNick() + "!~" + client.getNick() + "@127.0.0.1.ip JOIN : " + channelName + "\r\n", client); // envoyer la validation du join
    this->displayClient(":127.0.0.1 353 " + client.getNick() + " = " + channelName + " :" + this->getAllUsersChannel(this->getChannel(channelName)) + "\r\n", client);                    // envoyer la liste des utilisateur 
    this->displayClient(":127.0.0.1 353 " + client.getNick() + " = " + channelName + " :" + ":End of /NAMES list.\r\n", client);
    this->sendToAllUserInChannel(channelName, ":" + client.getNick() + "!~" + client.getNick() + "@127.0.0.1.ip JOIN :" + channelName + "\r\n", client);    // envoyer que l'utilisateur a join a tout les client
}


// Nom des channel incorrect = :lair.nl.eu.dal.net 403 jbe ASV :No such channel

void    server::joinFun(Client & client, std::vector<std::string> clientInput) {
    const std::string channelName = std::string(clientInput[1]);
    if (!checkNameChannel(channelName)) {
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channelName + " :No such channel\r\n",client);
        return;
    }
    //Check if channel exist
    for (std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++)
    {
        if ((it)->getChannelName() == channelName)
        {
            if (!(it)->addUser(client))
                return;

            //Sending a Topic message to client to confirm joining.
            this->displayClient(":127.0.0.1 332 " + client.getNick() + (*it).getChannelName() + " :Joined topic.\r\n", client);
            // send client lst on channel
            return;
        }
    }
    //If not, then create it
    Channel channel(channelName);
    channel.addUser(client);
    channel.setOp(client);

    //Sending a Topic message to client to confirm joining.
    this->displayClient(":127.0.0.1 332 " + client.getNick() + channel.getChannelName() + " :Created topic.\r\n", client);
    _ChannelList.push_back(channel);
    this->welcomeToChannel(client, channelName);

}

void    server::initFunLst(void)
{
    this->_FunLst["MODE"] = &server::modeFun;
    this->_FunLst["PING"] = &server::pingFun;
    this->_FunLst["NICK"] = &server::nickFun;
    this->_FunLst["USER"] = &server::userFun;
    this->_FunLst["PASS"] = &server::passFun;
    this->_FunLst["CAP"] =  &server::capFun;
    this->_FunLst["JOIN"] =  &server::joinFun;
    this->_FunLst["QUIT"] =  &server::quitFun;
    // this->_FunLst["/ban"] = &server::;
    // this->_FunLst["/unban"] = &server::;
    // this->_FunLst["/exit"] = &server::;
    // this->_FunLst["/silence"] = &server::;
    // this->_FunLst["/unsilence"] = &server::;
}

//  Server Funct

void    server::displayClient(std::string   msg, Client client) {
    send(client.getCS(), msg.c_str(), msg.size(), 0);
}

void    server::sendChannelMessage(Client & client, std::vector<std::string> clientInput)
{
    for (std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++)
    {
        if ((it)->getChannelName() == client.getChannel())
        {
            std::string res;
            for (std::vector<std::string>::const_iterator it = clientInput.begin(); it != clientInput.end(); ++it)
                res += *it + ' ';
            (it)->sendMessage(client, res);
            return;
        }
    }
    
    this->displayClient(":127.0.0.1 421 " + client.getNick() + " " + clientInput[0] + " :Unknow command\r\n", client);
}

void        server::execInput(std::vector<std::string> clientInput, Client & client) {
        Fun fun = _FunLst[clientInput[0]];
        if (fun) 
            (this->*fun)(client, clientInput);
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
        std::cout << "#" << it->getChannelName() << std::endl;
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

bool checkNonAlphanumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        if (!std::isalnum(*it) && (*it) != '_')
            return true;
    return false;
}

bool    checkNameChannel(const std::string& str) {
    if (str.empty() || str[0] != '#')
        return false;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        if (!std::isprint(*it))
            return false;
    return true;
}