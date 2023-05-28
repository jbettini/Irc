/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fun.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:30:33 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/28 19:33:59 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"


void    server::banFun(Client & client, std::vector<std::string> clientInput) {
    std::string channelName = clientInput[1];
    std::string userToBan;
    Channel  &   channel = this->getChannel(channelName);
    if (clientInput.size() == 3 && clientInput[2] == "+b") {
        std::cout << "In ban fun exception" << std::endl;
        // afficher la liste des ban
        return ;
    }
    // :127.0.0.1 482 jbettini_ #42 :You're not channel operator

    // :127.0.0.1 482 jbettini_ #42 :You must have channel halfop access or above to set channel mode b
    if (!(this->channelExist(channelName)))
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channelName + " :No such channel\r\n",client);
    else if (!(channel.isOp(client.getNick())))
        this->displayClient(":127.0.0.1 482 " + client.getNick() + " " + channelName + " :You're not channel operator\r\n", client);
    else {
            userToBan = extractUsernameModeFormat(clientInput[3]);
    //     if (checkNonAlphanumeric(userToBan) || userToBan.size() > 9 || channel.isBannedNick(userToBan))
    //         return ;
    //     else {
    //         std::string clientNick = client.getNick();
    //         channel.addToBanList(userToBan);
    //         sendToAllUserInChannel(channel.getChannelName(), clientNick + "!~" + clientNick + "@127.0.0.1 MODE " + channelName + " +b " + clientInput[3] + "\r\n", client);
    //         this->displayClient(clientNick + "!~" + clientNick + "@127.0.0.1 MODE " + channelName + " +b " + clientInput[3] + "\r\n", client);
    //     }
    }
    std::cout << "out ban fun " << std::endl;
}

void    server::modeFun(Client & client, std::vector<std::string> clientInput) {
    std::string mode = getMode(clientInput);
    std::cout << " mode = -" << mode << "-"<< std::endl;
    if (mode == "+i")
        this->displayClient(":127.0.0.1 221 " + client.getNick() + " :+i\r\n", client);
    else if (mode == "-i")
        this->displayClient(":127.0.0.1 221 " + client.getNick() + " :-i\r\n", client);
    else if (mode == "+b")
        this->banFun(client, clientInput);
    // else if (mode == "-b")

}

void    server::pingFun(Client & client, std::vector<std::string> clientInput) {
    (void)clientInput;
    this->displayClient("PONG :127.0.0.1\r\n", client);
}

void    server::nickFun(Client & client, std::vector<std::string> clientInput) {
    std::cout << "in NICK" << std::endl;
    if (clientInput.size() == 1) {
        this->displayClient(":127.0.0.1 431 " + client.getNick() + " :No nickname given\n\r", client);
        return ;
    }
    std::vector<Client>::iterator it;
    const std::string nick = std::string(clientInput[1]);
    if (nick.size() > 9)
    {
        this->displayClient(":127.0.0.1 432 " + client.getNick() + " " + nick + " :Nickname is too long (>9).\r\n", client);
        return;
    }
    else if (checkNonAlphanumeric(nick)) {
        this->displayClient(":127.0.0.1 432 " + client.getNick() + " " + nick + " :Nickname contain undefine caracter.\r\n", client);
        return ;
    }
    for (it = this->_ClientList.begin(); it != this->_ClientList.end(); it++) {
        if ((*it).getNick() == nick) {
            this->displayClient(":127.0.0.1 433 " + client.getNick() + " " + nick + " :Nickname is already in use.\r\n", client);
            return;
        }
    }
    if (client.getWelcome() == 1)
        this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1 NICK :" + nick + "\r\n", client);
    client.setNick(nick);
    //Complete connexion.
    if (client.isSetup() && client.getWelcome() == 0)
        this->welcomeMsg(client);
}

void    server::userFun(Client & client, std::vector<std::string> clientInput) {

    if (clientInput.size() == 1) {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " USER :Not enough parameters\r\n", client);
        return ;
    }
    client.setUsername(clientInput[1]);
    //Complete connexion.
    if (client.isSetup() && client.getWelcome() == 0) {
        this->welcomeMsg(client);
    }
}

void    server::passFun(Client & client, std::vector<std::string> clientInput)  {

    if (clientInput.size() == 1)  {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " PASS :Not enough parameters\r\n", client);
        return ;
    }
    const std::string pass = std::string(clientInput[1]);
    if (client.getPass() == 1)
        this->displayClient(":127.0.0.1 462 " + client.getNick() + " :You may not reregister.\r\n", client);
    else if (pass == this->_password)
        client.setPass(1);
    else if (client.getNick() == "admin" && pass == "admin")
    {
        client.setPass(1);
        client.setOp(true);
    }
    else
        this->displayClient(":127.0.0.1 464 " + client.getNick() + " :Incorrect Password.\r\n", client);
    
    //Complete connexion.
    if (client.isSetup() && client.getWelcome() == 0) {
        this->welcomeMsg(client);
    }
}


void        server::capFun(Client & client, std::vector<std::string> clientInput) {
    std::vector<std::string>    tmp = makeVecKey(clientInput, "PASS");
    if (tmp != clientInput)
        (this->*_FunLst[tmp[0]])(client, tmp);
    tmp = makeVecKey(clientInput, "USER");
    if (tmp != clientInput)
        (this->*_FunLst[tmp[0]])(client, tmp);
    tmp = makeVecKey(clientInput, "NICK");
    if (tmp != clientInput)
        (this->*_FunLst[tmp[0]])(client, tmp);
}

void    server::joinFun(Client & client, std::vector<std::string> clientInput) {

    if (clientInput.size() == 1)  {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " PASS :Not enough parameters\r\n", client);
        return ;
    }
    const std::string channelName = std::string(clientInput[1]);
    if (!checkNameChannel(channelName)) {
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channelName + " :No such channel\r\n",client);
        return;
    }
    else if (this->channelExist(channelName) && this->getChannel(channelName).isBanned(client.getNick())) {
        this->displayClient(":127.0.0.1 474 " + client.getNick() + " " + channelName + " :Cannot join channel (+b)\r\n", client);
        return ;
    }
    //Check if channel exist
    for (std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++)
    {
        if (this->channelExist(channelName))
        {
            if (!(*it).addUser(client)) 
                return;
            else
                this->welcomeToChannel(client, channelName);
            return;
        }
    }
    //If not, then create it
    Channel channel(channelName);
    channel.addUser(client);
    channel.setOp(client.getNick());
    this->_ChannelList.push_back(channel);
    this->welcomeToChannel(client, channelName);

}

void    server::privmsgFun(Client & client, std::vector<std::string> clientInput) {
    if (clientInput.size() == 2)
        this->displayClient(":127.0.0.1 412 " + client.getNick() + " :Cannot text to send\r\n", client);
    else if (clientInput.size() == 1)
        this->displayClient(":127.0.0.1 412 " + client.getNick() + " :No recipient given (PRIVMSG)\r\n", client);
    else if (checkNameChannel(clientInput[1]) && this->channelExist(clientInput[1])) {
        // if is ban or not
        if (this->getChannel(clientInput[1]).isBanned(client.getNick()))
            this->displayClient(":127.0.0.1 474 " + client.getNick() + " " + clientInput[1] + " :Cannot send to channel\r\n", client);
        else 
            sendToAllUserInChannel(clientInput[1], ":" + client.getNick() + "!~" + client.getNick() + "@127.0.0.1 PRIVMSG " + clientInput[1] + " " + catVecStr(clientInput, 3) + "\r\n", client);
    }
    else if (this->checkUserExist(clientInput[1]))
        this->displayClient(":" + client.getNick() + "!~" + client.getNick() + "@127.0.0.1 PRIVMSG " + clientInput[1] + " " + catVecStr(clientInput, 3) + "\r\n", client);
    else
        this->displayClient(":127.0.0.1 401 " + client.getNick() + " " + clientInput[1] + " :No such nick/channel\r\n", client);
}

void    server::initFunLst(void)
{
    this->_FunLst["PING"] = &server::pingFun;
    this->_FunLst["NICK"] = &server::nickFun;
    this->_FunLst["USER"] = &server::userFun;
    this->_FunLst["PASS"] = &server::passFun;
    this->_FunLst["CAP"] =  &server::capFun;
    this->_FunLst["PRIVMSG"] = &server::privmsgFun;
    this->_FunLst["JOIN"] =  &server::joinFun;
    this->_FunLst["MODE"] = &server::modeFun;
    // this->_FunLst["QUIT"] =  &server::quitFun;
    // this->_FunLst["/unban"] = &server::;
    // this->_FunLst["/exit"] = &server::;
    // this->_FunLst["/silence"] = &server::;
    // this->_FunLst["/unsilence"] = &server::;
}