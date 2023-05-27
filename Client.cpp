/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 18:51:52 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/27 01:57:58 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

struct ThreadData {
    int value;
};

Client::Client(void) :    _ClientSocket(0), _type(USER), _pollFd(-1), _username("*"), _nick("*"), _pass(0), _welcome(0), _admin(false) {}

Client::Client(int socketNum, int pollFd) :   _ClientSocket(socketNum), _type(USER), _pollFd(pollFd), _username("*"), _nick("*"), _pass(0), _welcome(0), _admin(false) {
}

// void    *ping(void * args)
// {
//     ThreadData* data = static_cast<ThreadData*>(args);
//     int fd = data->value;
//     while(1)
//     {
//         std::cout << "ping" << fd << std::endl;
//         send(fd, "PING 127.0.0.1\r\n", 17, 0);
//         sleep(10);
//     }
// }

Client &    Client::operator=(const Client & rhs) {
    
    this->_ClientSocket = rhs._ClientSocket;
    this->_type = rhs._type;
    this->_pollFd = rhs._pollFd;
    this->_username = rhs._username;
    this->_nick = rhs._nick;
    this->_myThread = rhs._myThread;
    this->_pass = rhs._pass;
    this->_welcome = rhs._welcome;
    this->_admin = rhs._admin;
    this->_allChannel = rhs._allChannel;
    return *this;
}

bool Client::isSetup(void) const {
    if (this->_username == "*" || this->_nick == "" || this->_nick == "*"|| this->_pass == 0)
        return (0);
    return (1);
}

// void Client::startPinging()
// {
//     ThreadData data;
//     data.value = _ClientSocket;

//     pthread_create(&this->_myThread, NULL, ping, &data);
// }

Client::Client(const Client & rhs) {
    *this = rhs;
}

Client::~Client(void) {};

std::string     Client::getNick(void){
            //std::cout << " THIS NICK = " << this->_nick << std::endl;
    return (this->_nick);
}