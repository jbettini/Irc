/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mySocket.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoudin <mgoudin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/10 17:03:34 by mgoudin           #+#    #+#             */
/*   Updated: 2023/05/17 19:57:16 by mgoudin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <map>
#include "Client.hpp"

#define MAX_CLIENTS 10  
#define BUFFER_SIZE 1024

class IRCServer {
private:
    int serverSocket;  
    int port;  
    std::string password;  
    std::vector<Client> clients;  
    struct pollfd fds[MAX_CLIENTS + 1];

public:
    IRCServer(int _port, const std::string& _password) : port(_port), password(_password) {}

    void run() {
        if (!initializeServer()) {
            std::cerr << "Erreur lors de l'initialisation du serveur." << std::endl;
            return;
        }

        memset(fds, 0, sizeof(fds));
        fds[0].fd = serverSocket;
        fds[0].events = POLLIN;

        while (true) {
            int pollResult = poll(fds, MAX_CLIENTS + 1, -1);
            if (pollResult < 0) {
                std::cerr << "Erreur lors de la fonction poll : " << strerror(errno) << std::endl;
                break;
            }

            if (pollResult == 0)
                continue;

            if (fds[0].revents & POLLIN) {
                acceptNewClient();
                fds[0].revents = 0;
            }

            for (size_t i = 0; i < clients.size(); ++i) {
                int id = clients[i].getFdId();
                if (fds[id].revents & POLLIN) {
                    if (!handleClientInput(i)) {
                        closeClient(i);
                        break;
                    }
                }
            }
        }

        cleanup();
    }

private:
    bool initializeServer() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Erreur lors de la création du socket : " << strerror(errno) << std::endl;
            return false;
        }

        int reuse = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
            std::cerr << "Erreur lors du paramétrage du socket : " << strerror(errno) << std::endl;
            close(serverSocket);
            return false;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Erreur lors du bind du socket : " << strerror(errno) << std::endl;
            close(serverSocket);
            return false;
        }

        if (listen(serverSocket, MAX_CLIENTS) < 0) {
            std::cerr << "Erreur lors de la mise en écoute du socket : " << strerror(errno) << std::endl;
            close(serverSocket);
            return false;
        }

        return true;
    }

    void acceptNewClient() {
        struct sockaddr_storage clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket < 0) {
            std::cerr << "Erreur lors de l'acceptation d'un client : " << strerror(errno) << std::endl;
            return;
        }

        int i;
        for (i = 1; i < MAX_CLIENTS + 1; i++)
        {
            if (fds[i].fd == 0) {
                fds[i].fd = clientSocket;
                fds[i].events = POLLIN;
                break;
            }
        }
        Client client(clientSocket, i);
        clients.push_back(client);

        std::cout << "Nouveau client connecté." << std::endl;
        std::string welcomeMsg = "Bienvenue sur le serveur IRC.\n\r";
        send(clientSocket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
    }

    bool handleClientInput(size_t clientIndex) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        
        int bytesRead = recv(clients[clientIndex].getSocketId(), buffer, sizeof(buffer) - 1, 0);
  
        if (bytesRead <= 0) {
            if (bytesRead < 0)
                std::cerr << "Erreur lors de la réception des données du client : " << strerror(errno) << std::endl;
            return false;
        }

        buffer[bytesRead] = '\0';
        std::string clientMsg(buffer);
        processIRCMessage(clientIndex, clientMsg);

        return true;
    }

    void killCmd(int clientSocket, std::vector<std::string> args)
    {
        std::cout << "killCmd" << std::endl;
        (void)args;
        (void)clientSocket;
    }

    void processIRCMessage(int clientSocket, const std::string& message) {
        std::istringstream iss(message);
        std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

        typedef void (IRCServer::*Fun)(int, std::vector<std::string>);
        std::map<std::string, Fun> funLst;

        if (tokens.empty())
            return;

        funLst["kill"] = &IRCServer::killCmd;

        std::string cmd = tokens[0];
        Fun fun = funLst[cmd];
        (this->*fun)(clientSocket, tokens);
    }

    void closeClient(size_t clientIndex) {
        int clientSocket = clients[clientIndex].getSocketId();
        close(clientSocket);
        clients.erase(clients.begin() + clientIndex);

        for (int i = 1; i < MAX_CLIENTS + 1; i++)
        {
            if (fds[i].fd == clientSocket) {
                fds[i].fd = 0;
                break;
            }
        }
        std::cout << "Client déconnecté." << std::endl;
    }

    void cleanup() {
        for (size_t i = 0; i < clients.size(); ++i) {
            close(clients[i].getSocketId());
        }
        close(serverSocket);
        clients.clear();
    }
};
