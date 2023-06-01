/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoudin <mgoudin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 18:26:29 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/30 17:28:51 by mgoudin          ###   ########.fr       */
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
        bool            deop(std::string   & client);
        bool            unban(std::string   & client);

        std::string     &    getChannelName(void) {
                    return (this->_nameChannel);
        }

        std::vector<Client>    &     getChannelUser(void) {
                    return (this->_Users);
        }

        int getCapacity(void) {
            return (this->_Users.size());
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

        std::string  getTopic(void) const {
            return (this->_topic);
        }

        int  getClientLimit(void) const {
            return (this->_clientLimit);
        }

        bool  checkPassword(std::string pass) const {
            return (pass == _password);
        }

        bool        canUserChangeTopic(void) const {
            return (this->_usersCanChangeTopic);
        }

        bool        isPasswordRequired(void) const {
            return (this->_isPasswordRequired);
        }

        bool        isClientLimitActivated(void) const {
            return (this->_isUserLimitActivated);
        }

        bool        isInviteOnly(void) const {
            return (this->_isInviteOnly);
        }

        void    setTopic(const std::string topic) {
            this->_topic = topic;
        }

        void setPassword(const std::string pass) {
            this->_password = pass;
        }

        void setClientLimit(const int n) {
            this->_clientLimit = n;
        }

        void    setCanUserChangeTopic(bool canUserChangeTopic) {
            this->_usersCanChangeTopic = canUserChangeTopic;
        }

        void    setClientLimitActivated(bool activated) {
            this->_isUserLimitActivated = activated;
        }

        void    setRequirePassword(bool require) {
            this->_isPasswordRequired = require;
        }

        void    setInviteOnly(bool inviteOnly) {
            this->_isInviteOnly = inviteOnly;
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

        void                addToInviteList(std::string client) {
            if (isInvited(client))
                return;
            this->_inviteUsers.push_back(client);
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

        bool                isInvited(std::string client)
        {
            for (std::vector<std::string>::iterator it = this->_inviteUsers.begin(); it != this->_inviteUsers.end(); it++)
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
            for (std::vector<std::string>::iterator it = this->_opUsers.begin(); it != this->_opUsers.end(); it++)
                if ((*it) == client) 
                    return (true);
            return (false);
        }

        void    sendMessage(Client& client, std::string msg)
        {
            for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end(); it++)
                if ((it)->getCS() != client.getCS())
                    send((*it).getCS(), msg.c_str(), msg.size(), 0);
        }

        bool    isUser(std::string nick) {
            for (std::vector<Client>::iterator it = this->_Users.begin(); it != this->_Users.end(); it++)
                if ((*it).getNick() == nick) 
                    return (true);
            return (false);
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
                std::vector<std::string>            _inviteUsers;
                std::string                         _topic;
                std::string                         _password;
                bool                                _isPasswordRequired;
                bool                                _usersCanChangeTopic;
                bool                                _isInviteOnly;
                bool                                _isUserLimitActivated;
                int                                 _clientLimit;

};

#endif