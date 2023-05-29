/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoudin <mgoudin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 18:26:29 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/29 17:14:47 by mgoudin          ###   ########.fr       */
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
        bool            addUser(Client& client);
        void            removeUser(Client   & client);
        void            unsilence(std::string   & client);
        void            deop(std::string  & client);
        bool            unban(std::string   & client);

        std::string     &    getChannelName(void) {
                    return (this->_nameChannel);
        }

        std::vector<Client>    &     getChannelUser(void) {
                    return (this->_Users);
        }

        std::vector<std::string> &  getSilentList(void) {
                    return (this->_silentUsers);
        }

        std::vector<std::string> &  getBannedList(void) {
                    return (this->_banedUsers);
        }

        std::vector<std::string> &  getOpUserList(void) {
                    return (this->_opUsers);
        }

        void                setOp(std::string client)
        {
            if (isOp(client))
                return;
            _opUsers.push_back(client);
        }

        void                addToBanList(std::string client) {
            if (isBanned(client))
                return;
            this->_banedUsers.push_back(client);
        }

        void                addToSilenceList(std::string client) {
            if (isSilent(client))
                return;
            this->_silentUsers.push_back(client);
        }

        bool                isBanned(std::string client)
        {
            for (std::vector<std::string>::iterator it = this->_banedUsers.begin(); it != this->_banedUsers.end(); it++)
                if ((*it) == client)
                    return (true);
            return (false);
        }

        bool                isBannedUser(std::string username)
        {
            for (std::vector<std::string>::iterator it = this->_banedUsers.begin(); it != this->_banedUsers.end(); it++)
                if ((*it) == username)
                    return (true);
            return (false);
        }

        bool                isSilent(std::string client)
        {
            for (std::vector<std::string>::iterator it = this->_silentUsers.begin(); it != this->_silentUsers.end(); it++)
                if ((*it) == client) return (true);
            return (false);
        }
        bool                isOp(std::string client)
        {
            // printVecStr(this->_opUsers);
            //std::cout << std::endl << client << " = client " << std::endl;
            for (std::vector<std::string>::iterator it = this->_opUsers.begin(); it != this->_opUsers.end(); it++)
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
                std::string                         _nameChannel;
                std::vector<Client>                 _Users;
                std::vector<std::string>            _silentUsers;
                std::vector<std::string>            _banedUsers;
                std::vector<std::string>            _opUsers;

};

#endif