/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:34:23 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/28 19:38:19 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void        server::welcomeMsg(Client & client){
    this->displayClient(":127.0.0.1 001 " +  client.getNick() + " :Vous êtes connecté avec succès à mon serveur\r\n", client);   
    client.setWelcome();
}

void    server::sendToAllUserInChannel(std::string channelName, std::string msg, Client & client) {
    for(std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++) {
        if ((*it).getChannelName() == channelName) {
            std::vector<Client> it2 = (*it).getChannelUser();
            for (std::vector<Client>::iterator it3 = it2.begin();it3 != it2.end();it3++)
                if ((*it3).getNick() != client.getNick())
                    this->displayClient(msg, *it3);
            break ;
        }
    }
}

void    server::welcomeToChannel(Client & client, std::string channelName) {

    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip JOIN : " + channelName + "\r\n", client); // envoyer la validation du join
    this->displayClient(":127.0.0.1 353 " + client.getNick() + " = " + channelName + " :" + this->getAllUsersChannel(this->getChannel(channelName)) + "\r\n", client);// envoyer la liste des utilisateur 
    this->displayClient(":127.0.0.1 366 " + client.getNick() + " " + channelName  + " :End of /NAMES list.\r\n", client);
    this->sendToAllUserInChannel(channelName, ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip JOIN :" + channelName + "\r\n", client);// envoyer que l'utilisateur a join a tout les client
}

void    server::displayClient(std::string   msg, Client client) {
    std::cout << "msg = " << msg << std::endl;
    send(client.getCS(), msg.c_str(), msg.size(), 0);
}