/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:18:29 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/21 20:46:41 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int main (int ac, char **av) {
    if (ac != 3)
        std::cout << "invalid number of argument" << std::endl;
    else {
        int port = std::atoi(av[1]);
        if (port < 1024 || port > 65535)
            std::cout << "invalid port, please select a port between 1024 and 65535" << std::endl;
        else {
            // try {
                server  serv(port,  av[2]);
                serv.run();
            // }
            // catch (const std::exception& e) {
            //     std::cout << e.what() << std::endl;
            // }
        }
    }
}