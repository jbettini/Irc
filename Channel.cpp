/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 19:27:01 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/30 15:48:11 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Channel::Channel(void) {}

Channel::Channel(std::string name) : _nameChannel(name) {}

Channel &    Channel::operator=(const Channel & rhs) {
    
    this->_nameChannel = rhs._nameChannel;
    this->_silentUsers = rhs._silentUsers;
    this->_banedUsers = rhs._banedUsers;
    this->_opUsers = rhs._opUsers;
    this->_Users = rhs._Users;
    return *this;
}

Channel::Channel(const Channel & rhs) {
    *this = rhs;
}

Channel::~Channel(void) {};

bool    Channel::addUser(Client& client)
{
    for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end(); it++)
        if ((*it) == client)
            return (false);
    if (isBanned(client.getUsername()))
    {
        std::string tmp = ":127.0.0.1 474 " + client.getNick() + " " + this->_nameChannel + " :Cannot join channel (+b)\r\n";
        send(client.getCS(), tmp.c_str(), tmp.size(), 0);
        return (false);
    }
    client.addChannelToCLient(_nameChannel);
    this->_Users.push_back(client);
    return (true);
}

void    Channel::removeUser(Client   & client) {
    for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end();it++) {
        if (it->getCS() == client.getCS()) {
            this->_Users.erase(it);
            break ;
        }
    }
}

void    Channel::unsilence(std::string   & client) {
    for (std::vector<std::string>::iterator it = this->_silentUsers.begin(); it != this->_silentUsers.end();it++) {
        if (*it == client) {
            this->_silentUsers.erase(it);
            break ;
        }
    }
}


bool    Channel::deop(std::string   & client) {
    for (std::vector<std::string>::iterator it = this->_opUsers.begin(); it != this->_opUsers.end();it++) {
        if (*it == client) {
            this->_opUsers.erase(it);
            return true;
        }
    }
    return false;
}

bool    Channel::unban(std::string   & client) {
    for (std::vector<std::string>::iterator it = this->_banedUsers.begin(); it != this->_banedUsers.end();it++) {
        if (*it == client) {
            this->_banedUsers.erase(it);
            return true;
        }
    }
    return false;
}