/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 18:26:29 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/26 22:57:42 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
    #define CHANNEL_HPP

#include "server.hpp"

class   Channel {
    public  :
        Channel(void);
        Channel(std::string name);
        Channel(const Channel & rhs);
        ~Channel(void);

        Channel &       operator=(const Channel & rhs);

        bool            checkNameChannel(const std::string& str);
        bool            addUser(Client& client);

        std::string         getChannelName(void) {
                    return (this->_nameChannel);
        }

        std::vector<Client>         getChannelUser(void) {
                    return (this->_Users);
        }

        // return false if user cant join (banned user or already in)
        bool                addUser(Client& client);
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

        void                removeUser(Client& client)
        {
            for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end(); it++)
                if (it->getCS() == client.getCS()) {
                    this->_Users.erase(it);
                    break;
            }
        }

        void                setOp(Client client)
        {
            if (isOp(client))
                return;
            _opUsers.push_back(client);
        }

        bool                isBanned(Client client)
        {
            for (std::vector<Client>::iterator it = this->_banedUsers.begin(); it != this->_banedUsers.end(); it++)
                if ((*it) == client) return (true);
            return (false);
        }
        bool                isSilent(Client client)
        {
            for (std::vector<Client>::iterator it = this->_silentUsers.begin(); it != this->_silentUsers.end(); it++)
                if ((*it) == client) return (true);
            return (false);
        }
        bool                isOp(Client client)
        {
            if (client.isAdmin()) 
                return (true);
            for (std::vector<Client>::iterator it = this->_opUsers.begin(); it != this->_opUsers.end(); it++)
                if ((*it) == client) 
                    return (true);
            return (false);
        }

        void    sendMessage(Client& client, std::string msg)
        {
            msg = client.getNick() + ": " + msg + "\r\n";
            for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end(); it++)
            {
                if ((it)->getCS() != client.getCS())
                    send((*it).getCS(), msg.c_str(), msg.size(), 0);
            }
        }

        //OPERATORS

        bool operator==(const Channel& other) const {
            return (_nameChannel == other._nameChannel);
        }

    private :
                std::string                 _nameChannel;
                std::vector<Client>         _Users;
                std::vector<Client>         _silentUsers;
                std::vector<Client>         _banedUsers;
                std::vector<Client>         _opUsers;

};

#endif