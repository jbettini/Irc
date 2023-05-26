/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 19:27:01 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/27 01:09:04 by jbettini         ###   ########.fr       */
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
    {
        if ((*it) == client)    
            return (false);
    }
    if (isBanned(client))
    {
        //TODO send error (IRC format)
        return (false);
    }
    client.addChannelToCLient(_nameChannel);
    _Users.push_back(client);
    return (true);
}