/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 16:44:55 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/15 16:58:01 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef S_HPP
    #define S_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <poll.h>

class server {

    public :
                server(void);
                server(server & rhs);
                ~server(void);
                server(int port, std::string pswd);

                server  &   operator=(server & rhs);

                int         getPort(void) {
                    return (this->_port);
                }
                std::string getPassword(void) {
                    return (this->_password);
                }

    private :
                int         _port;
                std::string _password; 
};

#endif