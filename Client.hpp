/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 19:31:19 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/24 21:14:55 by jbettini         ###   ########.fr       */
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
        int         getCS(void) { 
            return (this->_ClientSocket);
        }
        std::string         getUsername(void) { 
            return (this->_username);
        }
        void        setName(std::string name) {
            this->_username = name;
        }
        int         getPollFd(void) { 
            return (this->_pollFd);
        }
        pthread_t   &   getThread() {
            return this->_myThread;
        }
        std::string     getNick(void) { 
            return (this->_nick);
        }
        int             getPass(void) { 
            return (this->_pass);
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
};

#endif