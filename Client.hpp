/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 19:31:19 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/27 00:52:44 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include <thread>

#ifndef CLIENT_HPP
    #define CLIENT_HPP

#define OP      1
#define USER    0

class   Client {
    public  :
        Client(void);
        Client(const Client & rhs);
        Client(int socketNum, int pollFd);
        ~Client(void);

        Client &    operator=(const Client & rhs);


        void        startPinging(void);
        bool        isSetup(void) const;

        //setter

        void                            setNick(std::string nick) {
            this->_nick = nick;
        }
        void                            setUsername(std::string username) {
            this->_username = username;
        }
        void                            setPass(int i) {
            this->_pass = i;
        }
        void                            setWelcome() {
            this->_welcome++;
        }
        void                            setOp(bool isOp)
        {
            this->_admin = isOp;
        }
        void                            addChannelToCLient(std::string channel)
        {
            this->_allChannel.push_back(channel);
        }

        //geter

        int                             getCS(void) const{ 
            return (this->_ClientSocket);
        }
        int                             getType(void) { 
            return (this->_type);
        }
        int                             getPollFd(void) { 
            return (this->_pollFd);
        }
        std::string                     getUsername(void) { 
            return (this->_username);
        }
        std::string                     getNick(void);

        std::vector<std::string>        getChannel(void) { 
            return (this->_allChannel);
        }
        pthread_t   &                   getThread() {
            return this->_myThread;
        }
        int                             getPass(void) { 
            return (this->_pass);
        }
        int                             getWelcome(void) { 
            return (this->_welcome);
        }
        bool                            isAdmin(void) { 
            return (this->_admin);
        }
        std::vector<std::string> getAllChannel(void){
            return this->_allChannel;
        }
        //OPERATORS

        bool operator==(const Client& other) const {
            return (_username == other._username && _pollFd == other._pollFd);
        }

    private :
            int                         _ClientSocket;
            int                         _type;
            int                         _pollFd;
            std::string                 _username;
            std::string                 _nick;
            pthread_t                   _myThread;
            int                         _pass;
            int                         _welcome;
            bool                        _admin;
            std::vector<std::string>    _allChannel;
};

#endif