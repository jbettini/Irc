/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 16:44:55 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/17 20:03:48 by jbettini         ###   ########.fr       */
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
#include "Client.hpp"

class   server {

    public :
                server(void);
                server(server & rhs);
                ~server(void);
                server(int port, std::string pswd);

                void        run(void);
                void        init_socket(void);

                server  &   operator=(server & rhs);

                int         getPort(void) {
                    return (this->_port);
                }
                std::string getPassword(void) {
                    return (this->_password);
                }

                int     MAX_CLIENTS;
                int     MAX_BUFFER_SIZE;

    private :
                int                     _socket;
                int                     _port;
                std::string             _password; 
                struct  sockaddr_in     _serverAddress;
                std::vector<Client>     _ClientList;

};

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