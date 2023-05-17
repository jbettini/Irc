/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 18:51:52 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/17 20:26:43 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Client::Client(void) :    _ClientSocket(0), _type(USER), _pollFd(-1), _username("default"), _channel("General"){}

Client::Client(int socketNum, int pollFd) :   _ClientSocket(socketNum), _type(USER), _pollFd(pollFd), _username("default"), _channel("General") {}

// Client::Client(const Client &rhs) : _clientSocket(rhs._clientSocket), _type(rhs._type), _pollFd(rhs._pollFd), _username(rhs._username), _channel(rhs._channel) {}

Client &    Client::operator=(const Client & rhs) {
    
    this->_ClientSocket = rhs._ClientSocket;
    this->_type = rhs._type;
    this->_pollFd = rhs._pollFd;
    this->_username = rhs._username;
    this->_channel = rhs._channel;
    return *this;
}

Client::Client(const Client & rhs) {
    *this = rhs;
}

Client::~Client(void) {};