/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoudin <mgoudin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 19:31:19 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/26 16:21:37 by mgoudin          ###   ########.fr       */
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

        void        setNick(std::string nick) {
            this->_nick = nick;
        }
        void        setUsername(std::string username) {
            this->_username = username;
        }
        void        setPass(int i) {
            this->_pass = i;
        }
        void        setWelcome() {
            this->_welcome++;
        }
        void             setOp(bool isOp)
        {
            this->_isOp = isOp;
        }
        void         setChannel(std::string channel)
        {
            this->_channel = channel;
        }

        //geter

        int         getCS(void) const{ 
            return (this->_ClientSocket);
        }
        int         getType(void) { 
            return (this->_type);
        }
        int         getPollFd(void) { 
            return (this->_pollFd);
        }
        std::string         getUsername(void) { 
            return (this->_username);
        }
        std::string     getNick(void);
        std::string     getChannel(void) { 
            return (this->_channel);
        }
        pthread_t   &   getThread() {
            return this->_myThread;
        }
        int             getPass(void) { 
            return (this->_pass);
        }
        int             getWelcome(void) { 
            return (this->_welcome);
        }
        bool             isOp(void) { 
            return (this->_isOp);
        }
        std::vector<std::string> getAllChannel(void){
            return this->_allChannel;
        }

        //OPERATORS

        bool operator==(const Client& other) const {
            return (_username == other._username && _pollFd == other._pollFd);
        }

    private :
            int             _ClientSocket;
            int             _type;
            int             _pollFd;
            std::string     _username;
            std::string     _nick;
            pthread_t       _myThread;
            int             _pass;
            int             _welcome;
            bool            _isOp;
            std::string    _channel;
            std::vector<std::string> _allChannel; 
};

#endif