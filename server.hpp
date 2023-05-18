/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 16:44:55 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/18 21:22:59 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef S_HPP
    #define S_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <list>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"

class   server {

    public :
                server(void);
                server(server & rhs);
                ~server(void);
                server(int port, std::string pswd);

                void        run(void);
                void        init_socket(void);
                void        disconnectClient(struct pollfd & ClientFd);
                void        addChannel(std::string name);
                void        printChannel(void);
                void        parseInput(std::vector<std::string> clientInput, Client client);

                server  &   operator=(server & rhs);

                int         getPort(void) {
                    return (this->_port);
                }
                std::string getPassword(void) {
                    return (this->_password);
                }
                Client      getClient(int newClient) {
                    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
                        if (it->getCS() == newClient)
                            return (*it);
                }

                int     MAX_CLIENTS;
                int     MAX_BUFFER_SIZE;

    private :
                int                     _socket;
                int                     _port;
                std::string             _password; 
                struct  sockaddr_in     _serverAddress;
                std::vector<Client>     _ClientList;
                std::vector<Channel>    _ChannelList;

};

std::vector<std::string>    splitBuffer(char *buffer, char delimiter);
                class   serverException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from server !");
                    }
                };
                class   pollException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from poll !");
                    }
                };
                class   socketException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from  socket !");
                    }
                };

                class   bindException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from  bind !");
                    }
                }; 

                class   listenException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from listen !");
                    }
                };

                class   acceptException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from accept !");
                    }
                };

                class   recvException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from recv !");
                    }
                };

                class   sendException : public std::exception {
                    virtual const char* what() const throw() {
                        return ("Error : from  !");
                    }
                };

#endif