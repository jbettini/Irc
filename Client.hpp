/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 19:31:19 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/25 05:17:36 by jbettini         ###   ########.fr       */
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

        //seter

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

        //geter

        int         getCS(void) { 
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
        std::string     getNick(void) { 
            return (this->_nick);
        }
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

    private :
            int         _ClientSocket;
            int         _type;
            int         _pollFd;
            std::string _username;
            std::string _nick;
            std::string _channel;
            pthread_t   _myThread;
            int         _pass;
            int         _welcome;
};

#endif