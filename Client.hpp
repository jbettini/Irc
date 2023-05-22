/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 19:31:19 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/22 15:55:30 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

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

    private :
            int         _ClientSocket;
            int         _type;
            int         _pollFd;
            std::string _username;
            std::string _channel;


};

#endif