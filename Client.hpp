/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 19:31:19 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/17 21:16:01 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

#define OP      1
#define USER    0

class   Client {
    public:
        Client(void);
        Client(const Client & rhs);
        Client(int socketNum, int pollFd);
        ~Client(void);

        Client &    operator=(const Client & rhs);

        int         getCS(void) {
                    return (this->_ClientSocket);
                }

    private:
            int         _ClientSocket;
            int         _type;
            int         _pollFd;
            std::string _username;
            std::string _channel;


};