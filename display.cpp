/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:34:23 by jbettini          #+#    #+#             */
/*   Updated: 2023/06/01 23:47:17 by jbettini         ###   ########.fr       */
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

void    server::sendToAllUser(std::string channelName, std::string msg) {
    for(std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++) {
        if ((*it).getChannelName() == channelName) {
            std::vector<Client> it2 = (*it).getChannelUser();
            for (std::vector<Client>::iterator it3 = it2.begin();it3 != it2.end();it3++)
                    this->displayClient(msg, *it3);
            break ;
        }
    }
}

void    server::welcomeToChannel(Client & client, std::string channelName) {

    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip JOIN : " + channelName + "\r\n", client);
    this->displayClient(":127.0.0.1 353 " + client.getNick() + " = " + channelName + " :" + this->getAllUsersChannel(this->getChannel(channelName)) + "\r\n", client);
    this->displayClient(":127.0.0.1 366 " + client.getNick() + " " + channelName  + " :End of /NAMES list.\r\n", client);
    this->sendToAllUserInChannel(channelName, ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip JOIN :" + channelName + "\r\n", client);
}

void    server::displayClient(std::string   msg, Client client) {
    std::cout << "msg = " << msg << "----" << std::endl;
    send(client.getCS(), msg.c_str(), msg.size(), 0);
}

void    printfChannel(Channel channel){
    std::cout << "channel = " << channel.getChannelName() << " - channel user size : " << channel.getChannelUser().size() << std::endl << std::endl;
    std::cout << "Banned User" << std::endl;
    printVecStr(channel.getBannedList());
    std::cout << std::endl;
    std::cout << "op User" << std::endl;
    printVecStr(channel.getOpUserList());
    std::cout << std::endl;
    std::cout << "silentUser" << std::endl;
    printVecStr(channel.getSilentList());
    std::cout << std::endl;
}

void    server::printChannel(void) {
        std::cout << "Channel List : " << std::endl;
    for (std::vector<Channel>::iterator it = _ChannelList.begin(); it != _ChannelList.end(); it++)
        std::cout << "#" << it->getChannelName() << std::endl;
}

void printVecStr(std::vector<std::string> strings) {
    for (size_t i = 0; i < strings.size(); ++i)
        std::cout << "\" str = " + strings[i] + "\""<< std::endl;
}

void printClient(std::vector<Client> strings) {
    for (size_t i = 0; i < strings.size(); ++i) {
        std::cout << "ClientSocket : " << strings[i].getCS()<< std::endl;
        std::cout << "ClientUsername : " << strings[i].getUsername() << std::endl;
        std::cout << "ClientNick : " << "\"" << strings[i].getNick() << "\"" <<std::endl;
    }
}