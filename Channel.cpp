/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 19:27:01 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/28 20:10:47 by jbettini         ###   ########.fr       */
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
    std::cout << "In addUser "<< std::endl;
    for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end(); it++)
        if ((*it) == client)    
            return (false);
    std::cout << "IN BAN addUser " << std::endl;
    if (isBanned(client.getNick()))
    {
        //TODO send error (IRC format)

        return (false);
    }
    client.addChannelToCLient(_nameChannel);
    this->_Users.push_back(client);
    std::cout << "Out addUser "<< std::endl;
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

void    Channel::deop(std::string   & client) {
    for (std::vector<std::string>::iterator it = this->_opUsers.begin(); it != this->_opUsers.end();it++) {
        if (*it == client) {
            this->_opUsers.erase(it);
            break ;
        }
    }
}

void    Channel::unban(std::string   & client) {
    for (std::vector<std::string>::iterator it = this->_banedUsers.begin(); it != this->_banedUsers.end();it++) {
        if (*it == client) {
            this->_banedUsers.erase(it);
            break ;
        }
    }
}