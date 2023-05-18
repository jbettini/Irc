/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 18:26:29 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/18 19:58:08 by jbettini         ###   ########.fr       */
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

        Channel &    operator=(const Channel & rhs);

        std::string         getName(void) {
                    return (this->_nameChannel);
        }

    private :
                std::string                 _nameChannel;
                std::vector<std::string>    _silentUsers;
                std::vector<std::string>    _banedUsers;

};

#endif