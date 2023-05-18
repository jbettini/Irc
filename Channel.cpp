/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 19:27:01 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/18 19:34:38 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Channel::Channel(void) {}

Channel::Channel(std::string name) : _nameChannel(name) {}

Channel &    Channel::operator=(const Channel & rhs) {
    
    this->_nameChannel = rhs._nameChannel;
    this->_silentUsers = rhs._silentUsers;
    this->_banedUsers = rhs._banedUsers;
    return *this;
}

Channel::Channel(const Channel & rhs) {
    *this = rhs;
}

Channel::~Channel(void) {};
