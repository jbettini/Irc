/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fun.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:30:33 by jbettini          #+#    #+#             */
/*   Updated: 2023/06/02 19:42:16 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void    server::quitFun(Client & client, std::vector<std::string> clientInput) {
    if (clientInput.size() - 1 == 0)
        sendToAllClientChannel(client, clientInput, 0);
    else
        sendToAllClientChannel(client, clientInput, 1);
}

void    server::banFun(Client & client, std::vector<std::string> clientInput) {
    std::string channelName = clientInput[1];
    std::string userToBan;
    Channel  &   channel = this->getChannel(channelName);
    std::string mode = getMode(clientInput);
    if (!(this->channelExist(channelName)))
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channelName + " :No such channel\r\n",client);
    else if ((clientInput[2] == "+b" || clientInput[2] == "-b") && clientInput.size() == 3)
        this->banLstFun(client, clientInput);
    else if (!(channel.isOp(client.getNick()))) {
        this->displayClient(":127.0.0.1 482 " + channelName + " :You're not channel operator\r\n", client);
    }
    else {
        if (checkFormat(clientInput[3]))
            userToBan = extractUsernameModeFormat(clientInput[3]);
        else
            userToBan = this->getUsernameByNick(clientInput[3]);
        if (this->checkUsernameExist(userToBan) && channel.isOp(this->getClientWithUser(userToBan).getNick()))
            return ;
        else if (checkNonAlphanumeric(userToBan) || userToBan.size() > 9 )
            return ;
        else {
            std::string clientNick = client.getNick();
            std::string clientUsername = client.getUsername();
            if (mode == "+b") {
                if (channel.isBannedUser(userToBan))
                    return ;
                else
                    channel.addToBanList(userToBan);
            }
            else if (mode == "-b" && (!(channel.unban(userToBan))))
                return ;
            sendToAllUserInChannel(channelName, ":" + clientNick + "!~" + clientUsername + "@127.0.0.1 MODE " + channelName + " " + mode + " " + userToBan + "!*@*\r\n", client);
            this->displayClient(                ":" + clientNick + "!~" + clientUsername + "@127.0.0.1 MODE " + channelName + " " + mode + " " + userToBan + "!*@*\r\n", client);
        }
    }
}

void    server::opFun(Client & client, std::vector<std::string> clientInput) {
    std::string channelName = clientInput[1];
    std::string userToOp;
    Channel  &   channel = this->getChannel(channelName);
    std::string mode = getMode(clientInput);
    if (!(this->channelExist(channelName)))
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channelName + " :No such channel\r\n",client);
    else if (!(channel.isOp(client.getNick())))
        this->displayClient(":127.0.0.1 482 " + channelName + " :You're not channel operator\r\n", client);
    else {
        if (checkFormat(clientInput[3]))
            userToOp = extractUsernameModeFormat(clientInput[3]);
        else
            userToOp = clientInput[3];
        if (!(this->checkNickExist(userToOp)))
            return ;
        else if (checkNonAlphanumeric(userToOp) || userToOp.size() > 9 )
            return ;
        else {
            std::string clientNick = client.getNick();
            std::string clientUsername = client.getUsername();
            if (mode == "+o") {
                if (channel.isOp(userToOp))
                    return ;
                else
                    channel.setOp(userToOp);
            }
            else if (mode == "-o" && (!(channel.deop(userToOp))))
                    return ;
            sendToAllUserInChannel(channelName, ":" + clientNick + "!~" + clientUsername + "@127.0.0.1 MODE " + channelName + " " + mode + " " + userToOp + "\r\n", client);
            this->displayClient(                ":" + clientNick + "!~" + clientUsername + "@127.0.0.1 MODE " + channelName + " " + mode + " " + userToOp + "\r\n", client);
        }
    }
}

void    server::setTopicRestrictionFun(Client & client, std::vector<std::string> clientInput) {
    const std::string channel_str = clientInput[1];
    const std::string i_str = clientInput[2];

    // Check if channel exist
    if (!this->channelExist(channel_str))
    {
        this->displayClient(":127.0.0.1 403 " + channel_str + " " + i_str + " :No such channel\r\n", client);
        return;
    }
    
    Channel& channel = this->getChannel(channel_str);

    // If user is not op, he cant change channel restriction.
    if (!channel.isOp(client.getNick()))
    {
        this->displayClient(":127.0.0.1 482 " + client.getNick() + " " + channel.getChannelName() + " :" + "You're not channel operator\r\n", client);
        return;
    }

    // Set restriction
    if (i_str == "-t")
        channel.setCanUserChangeTopic(false);
    else
        channel.setCanUserChangeTopic(true);
        
    //Sending confirmation to client
    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " MODE " + channel.getChannelName() + " " + i_str + "\r\n", client);
}

void    server::handleModeiFun(Client & client, std::vector<std::string> clientInput) {
    const std::string channel_or_client_str = clientInput[1];
    const std::string i_str = clientInput[2];
    
    // If nick exist, client sent +i/-i for invisible mode
    if (this->checkNickExist(channel_or_client_str))
    {
        this->displayClient(":127.0.0.1 221 " + client.getNick() + " :" + i_str + "\r\n", client);
        return;
    }
    // else, client sent +i/-i for invite mode on channel;
    
    // Check if channel exist
    if (!this->channelExist(channel_or_client_str))
    {
        this->displayClient(":127.0.0.1 403 " + channel_or_client_str + " " + i_str + " :No such channel\r\n", client);
        return;
    }

    Channel& channel = this->getChannel(channel_or_client_str);

    // If user is not op, he cant change channel restriction.
    if (!channel.isOp(client.getNick()))
    {
        this->displayClient(":127.0.0.1 482 " + client.getNick() + " " + channel.getChannelName() + " :" + "You're not channel operator\r\n", client);
        return;
    }

    // Set restriction
    if (i_str == "-i")
        channel.setInviteOnly(false);
    else
        channel.setInviteOnly(true);
    //Sending confirmation to client
    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " MODE " + channel.getChannelName() + " " + i_str + "\r\n", client);
}

void    server::setPasswordRestrictionFun(Client & client, std::vector<std::string> clientInput) {
    const std::string channel_str = clientInput[1];
    const std::string k_str = clientInput[2];

    // Check if channel exist
    if (!this->channelExist(channel_str))
    {
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channel_str + " :No such channel\r\n", client);
        return;
    }
    
    Channel& channel = this->getChannel(channel_str);

    // If user is not op, he cant change channel restriction.
    if (!channel.isOp(client.getNick()))
    {
        this->displayClient(":127.0.0.1 482 " + client.getNick() + " " + channel.getChannelName() + " :" + "You're not channel operator\r\n", client);
        return;
    }

    // Set restriction
    if (k_str == "-k")
    {
        channel.setRequirePassword(false);
        return;
    }
    // if +k, check if user defined password
    if (clientInput.size() < 4)
    {
        // +k but no password was defined, just return;
        return;
    }

    const std::string password = clientInput[3];
    channel.setRequirePassword(true);
    channel.setPassword(password);

    //Sending confirmation to client
    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " MODE " + channel.getChannelName() + " " + k_str + " " + password + "\r\n", client);
}

void    server::setClientLimitRestrictionFun(Client & client, std::vector<std::string> clientInput) {
    const std::string channel_str = clientInput[1];
    const std::string l_str = clientInput[2];

    // Check if channel exist
    if (!this->channelExist(channel_str))
    {
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channel_str + " :No such channel\r\n", client);
        return;
    }
    
    Channel& channel = this->getChannel(channel_str);

    // If user is not op, he cant change channel restriction.
    if (!channel.isOp(client.getNick()))
    {
        this->displayClient(":127.0.0.1 482 " + client.getNick() + " " + channel.getChannelName() + " :" + "You're not channel operator\r\n", client);
        return;
    }

    // Set restriction
    if (l_str == "-l")
    {
        channel.setClientLimitActivated(false);
        this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " MODE " + channel.getChannelName() + " " + l_str + "\r\n", client);
        return;
    }
    // if +l, check if user defined client limit
    if (clientInput.size() < 4)
    {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " MODE +l :Not enough parameters\r\n", client);
        return;
    }

    try
    {
        const int maxClient = stod(clientInput[3]);
        if (maxClient < 1)
        {
            //maxClient is set to 0 or negative number, just return;
            return;
        }
        channel.setClientLimitActivated(true);
        channel.setClientLimit(maxClient);

        //Sending confirmation to client
        this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " MODE " + channel.getChannelName() + " " + l_str + " " + std::to_string(maxClient) + "\r\n", client);
    }
    catch (std::exception e)
    {
        //stod failed; maybe int max was reach, just return;
        return;
    }
}

void    server::banLstFun(Client & client, std::vector<std::string> clientInput) {
    //Check if enough params
    if (clientInput.size() < 3)
    {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " MODE b :Not enough parameters\r\n", client);
        return;
    }

    const std::string channel_str = clientInput[1];

    // Check if channel exist
    if (!this->channelExist(channel_str))
    {
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channel_str + " :No such channel\r\n", client);
        return;
    }
    
    Channel& channel = this->getChannel(channel_str);
    channel.sendBanLst(client);
}

void    server::modeLstFun(Client & client, std::vector<std::string> clientInput) {
    const std::string channel_str = clientInput[1];

    //Check if enough params, if not, user is only doing /mode
    if (clientInput.size() < 2)
    {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " MODE :Not enough parameters\r\n", client);
        return;
    }

    // Check if channel exist
    if (!this->channelExist(channel_str))
    {
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channel_str + " :No such channel\r\n", client);
        return;
    }

    // Send mode lst
    Channel& channel = this->getChannel(channel_str);
    this->displayClient(":127.0.0.1 324 " + client.getNick() + " " + channel_str + " " + channel.stringifyModeLst() +"\r\n", client);
}

void    server::modeFun(Client & client, std::vector<std::string> clientInput) {
    //If only 2args, it's /mode channel_name
    if (clientInput.size() < 3)
    {
        this->modeLstFun(client, clientInput);
        return;
    }
    std::string mode = getMode(clientInput);
    if (mode == "+i" || mode == "-i")
        this->handleModeiFun(client, clientInput);
    else if (mode == "+b" || mode == "-b")
        this->banFun(client, clientInput);
    else if (mode == "-o" || mode == "+o")
        this->opFun(client, clientInput);
    else if (mode == "-t" || mode == "+t")
        this->setTopicRestrictionFun(client, clientInput);
    else if (mode == "-k" || mode == "+k")
        this->setPasswordRestrictionFun(client, clientInput);
    else if (mode == "-l" || mode == "+l")
        this->setClientLimitRestrictionFun(client, clientInput);
    else if (mode == "b")
        this->banLstFun(client, clientInput);
}

void    server::pingFun(Client & client, std::vector<std::string> clientInput) {
    (void)clientInput;
    this->displayClient("PONG :127.0.0.1\r\n", client);
}

void    server::nickFun(Client & client, std::vector<std::string> clientInput) {
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
    // cree fonction qui va iterer sur channel list et modifier le nick 
    this->updateChannelListNick(this->_ChannelList, client.getNick(), nick);
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
    std::string user = clientInput[1];
    if (user.size() > 9)
    {
        this->displayClient(":127.0.0.1 432 " + client.getUsername() + " " + user + " :User is too long (>9).\r\n", client);
        return;
    }
    else if (checkNonAlphanumeric(user)) {
        this->displayClient(":127.0.0.1 432 " + client.getUsername() + " " + user + " :User contain undefine caracter.\r\n", client);
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
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " JOIN :Not enough parameters\r\n", client);
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
    else if (this->channelExist(channelName) && this->getChannel(channelName).isInviteOnly() && !this->getChannel(channelName).isInvited(client.getNick())) {
        this->displayClient(":127.0.0.1 474 " + client.getNick() + " " + channelName + " :Cannot join channel (+i)\r\n", client);
        return ;
    }
    else if (this->channelExist(channelName) && this->getChannel(channelName).isPasswordRequired()) {
        // If password isnt defined, or passsword is wrong
        if (clientInput.size() < 3 || !this->getChannel(channelName).checkPassword(clientInput[2]))
        {
            this->displayClient(":127.0.0.1 475 " + client.getNick() + " " + channelName + " :Cannot join channel (+k)\r\n", client);
            return ;
        }
    }
    else if (this->channelExist(channelName) && this->getChannel(channelName).isClientLimitActivated()) {
        // If client limit, check if channel is full
        if (this->getChannel(channelName).getCapacity() >= this->getChannel(channelName).getClientLimit())
        {
            this->displayClient(":127.0.0.1 475 " + client.getNick() + " " + channelName + " :Cannot join channel (+l)\r\n", client);
            return ;
        }
    }

    //Check if channel exist
    if (this->channelExist(channelName))
    {
        Channel& channel = this->getChannel(channelName);
        
        if (!(channel.addUser(client)))
            return;
        else
            this->welcomeToChannel(client, channelName);
        return;
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
        this->displayClient(":127.0.0.1 411 " + client.getNick() + " :No recipient given (PRIVMSG)\r\n", client);
    else if (checkNameChannel(clientInput[1]) && this->channelExist(clientInput[1])) {
        // if is ban or not
        if (this->getChannel(clientInput[1]).isBanned(client.getUsername()) || !(this->getChannel(clientInput[1]).isUser(client.getNick())))
            this->displayClient(":127.0.0.1 404 " + clientInput[1] + " :Cannot send to channel\r\n", client);
        else 
            sendToAllUserInChannel(clientInput[1], ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1 PRIVMSG " + clientInput[1] + " " + catVecStr(clientInput, 3) + "\r\n", client);
    }
    else if (this->checkNickExist(clientInput[1]))
        this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1 PRIVMSG " + clientInput[1] + " " + catVecStr(clientInput, 3) + "\r\n", client);
    else
        this->displayClient(":127.0.0.1 401 " + client.getNick() + " " + clientInput[1] + " :No such nick/channel\r\n", client);
}


void    server::whoFun(Client & client, std::vector<std::string> clientInput) {
    //This function only works for /who channel, maybe todo /who client

    if (clientInput.size() < 2)
        return;
    const std::string client_or_channel_str = clientInput[1];

    //Check if channel exist
    if (this->channelExist(client_or_channel_str))
    {
        Channel& channel = this->getChannel(client_or_channel_str);
        
        channel.sendWhoLst(client);
    }

    return ;
}

void    server::noticeFun(Client & client, std::vector<std::string> clientInput) {
    if (clientInput.size() == 2)
        this->displayClient(":127.0.0.1 412 " + client.getNick() + " :Cannot text to send\r\n", client);
    else if (clientInput.size() == 1)
        this->displayClient(":127.0.0.1 411 " + client.getNick() + " :No recipient given (NOTICE)\r\n", client);
    else if (checkNameChannel(clientInput[1]) && this->channelExist(clientInput[1])) {
        if (this->getChannel(clientInput[1]).isBanned(client.getUsername()) || !(this->getChannel(clientInput[1]).isUser(client.getNick())))
            return ;
        else 
            sendToAllUserInChannel(clientInput[1], ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1 NOTICE " + clientInput[1] + " " + catVecStr(clientInput, 3) + "\r\n", client);
    }
    else if (this->checkNickExist(clientInput[1]))
        this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1 NOTICE " + clientInput[1] + " " + catVecStr(clientInput, 3) + "\r\n", client);
    else
        return ;
}


void    server::topicFun(Client & client, std::vector<std::string> clientInput) {
    //No params case
    if (clientInput.size() == 1)
    {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " TOPIC :Not enough parameters\r\n", client);
        return;
    }
    
    const std::string channelName = std::string(clientInput[1]);
    Channel& channel = this->getChannel(channelName);
    //1 params case (just print)
    if (clientInput.size() == 2)
    {
        this->displayClient(":127.0.0.1 332 " + client.getNick() + " " + channel.getChannelName() + " :" + channel.getTopic() + "\r\n", client);
        this->displayClient(":127.0.0.1 333 " + client.getNick() + " " + channel.getChannelName() + " " + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1\r\n", client);
        return;
    }

    //2+ params case (set topic);
    
    std::string topicName = catVecStr(clientInput, 3);

    //If client isnt admin; isnt op in channel; and user cant change topic, return.
    if (!client.isAdmin() && !channel.isOp(client.getNick()) && !channel.canUserChangeTopic())
    {
        this->displayClient(":127.0.0.1 482 " + client.getNick() + " " + channel.getChannelName() + " :" + "You're not channel operator\r\n", client);
        return;
    }
    channel.setTopic(topicName);
    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " TOPIC " + channel.getChannelName() + " :" + topicName + "\r\n", client);
}

void    server::inviteFun(Client & client, std::vector<std::string> clientInput) {
    // Check if enought params (needs 2)
    if (clientInput.size() < 3)
    {
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " INVITE :" + "Not enough parameters\r\n", client);
        return;
    }
    
    const std::string invitedUser = std::string(clientInput[1]);
    const std::string channel_str = std::string(clientInput[2]);

    //Send error if nick doesnt exist
    if (!this->checkNickExist(invitedUser))
    {
        this->displayClient(":127.0.0.1 401 " + client.getNick() + " " + invitedUser + " :" + "No such nick/channel\r\n", client);
        return;
    }
    //Send error if channel doesnt exist
    if (!this->channelExist(channel_str))
    {
        this->displayClient(":127.0.0.1 401 " + client.getNick() + " " + channel_str + " :" + "No such nick/channel\r\n", client);
        return;
    }
    
    Client& invitedClient = this->getClientWithNick(invitedUser); 
    Channel& channel = this->getChannel(channel_str);
    channel.addToInviteList(invitedClient.getNick());
    
    // What Client that send invite get:
    this->displayClient(":127.0.0.1 341 " + client.getNick() + " " + invitedClient.getNick() + " " + channel.getChannelName() + "\r\n", client);
    this->displayClient(":127.0.0.1 401 NOTICE @" + channel.getChannelName() + " :" + client.getNick() + " invited " + invitedClient.getNick() + " into channel " + channel.getChannelName() + "\r\n", client);

    // What Client that receive invite get:
    this->displayClient(":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " INVITE " + invitedClient.getNick() + " :" + channel.getChannelName() + "\r\n", invitedClient);
}

void    server::kickFun(Client & client, std::vector<std::string> clientInput){
    std::string channelName = clientInput[1];
    std::string msg;

    if (clientInput.size() == 1) 
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " KICK :Not enough parameters\r\n", client);
    else if (!(this->channelExist(clientInput[1])))
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + clientInput[1] + " :No such channel\r\n",client);
    else if (!(this->getChannel(clientInput[1]).isUser(client.getNick())))
        this->displayClient(":127.0.0.1 442 " + client.getNick() + " " + clientInput[1] + " :You're not on that channel\r\n",client);
    else if (!(this->getChannel(channelName).isOp(client.getNick())))
            this->displayClient(":127.0.0.1 482 " + channelName + " :You're not channel operator\r\n", client);
    else {
        Channel  &   channel = this->getChannel(channelName);
        if (channel.isUser(clientInput[2]) && (!(channel.isOp(clientInput[2]))))  {
            Client       toKick     = this->getClientWithNick(clientInput[2]);
            if (clientInput[3] != ":")
                msg =  ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1 KICK " + channel.getChannelName() + " " + clientInput[2] + " :" + catVecStr(removeFirstCharacterIfColonIdx(clientInput, 4), 4) + "\r\n";
            else 
                msg =  ":" + client.getNick() + " KICK " + channel.getChannelName() + " " + toKick.getNick() + " :" + client.getNick() + "\r\n";
            this->displayClient(msg, toKick);
            sendToAllUserInChannel(channelName, msg, toKick);
            channel.removeUser(toKick);
        }
    }
}
        
void    server::partFun(Client & client, std::vector<std::string> clientInput) {
    if (clientInput.size() == 1)
        this->displayClient(":127.0.0.1 461 " + client.getNick() + " PART :Not enough parameters\r\n", client);
    else if (!(this->channelExist(clientInput[1])))
        this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + clientInput[1] + " :No such channel\r\n",client);
    else if (!(this->getChannel(clientInput[1]).isUser(client.getNick())))
        this->displayClient(":127.0.0.1 442 " + client.getNick() + " " + clientInput[1] + " :You're not on that channel\r\n",client);
    else {
        Channel& channel = this->getChannel(clientInput[1]);
        std::string msg;
        if (clientInput.size() > 2)
            msg =   ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip PART " + channel.getChannelName() + " :" + catVecStr(removeFirstCharacterIfColonIdx(clientInput, 3), 3) + "\r\n";
        else 
            msg =   ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1.ip PART " + channel.getChannelName() + " :" + client.getNick() + "\r\n";
        sendToAllUser(channel.getChannelName(), msg);
        client.removeChannel(channel.getChannelName());
        channel.removeUser(client);
        if (channel.getChannelUser().size() == 0)
            this->removeChannelToChannelList(channel.getChannelName());
    }
}

// void    server::partFun(Client & client, std::vector<std::string> clientInput) {
//     // Check if enought params (needs 2)
//     if (clientInput.size() < 2)
//     {
//         this->displayClient(":127.0.0.1 461 " + client.getNick() + " PART :Not enough parameters\r\n", client);
//         return;
//     }

//     const std::string channel_str = clientInput[1];
//     //Send error if channel doesnt exist
//     if (!this->channelExist(channel_str) || !client.isInChannel(channel_str))
//     {
//         this->displayClient(":127.0.0.1 403 " + client.getNick() + " " + channel_str + " :No such channel\r\n", client);
//         return;
//     }

//     Channel& channel = this->getChannel(channel_str);

//     //Remove user in channel
//     channel.removeUser(client);
//     //Remove channel in user
//     client.removeChannel(channel.getChannelName());

//     //Sending confirmation to client, and all user in channel
//     const std::string msg = ":" + client.getNick() + "!~" + client.getUsername() + "@127.0.0.1" + " PART " + channel.getChannelName() + "\r\n";
//     channel.sendMessage(client, msg);
//     this->displayClient(msg, client);
// }

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
    this->_FunLst["QUIT"] =  &server::quitFun;
    this->_FunLst["PART"] = &server::partFun;
    this->_FunLst["NOTICE"] = &server::noticeFun;
    this->_FunLst["WHO"] = &server::whoFun;
    this->_FunLst["WHOIS"] = &server::whoFun;
    this->_FunLst["TOPIC"] = &server::topicFun;
    this->_FunLst["INVITE"] = &server::inviteFun;
    this->_FunLst["KICK"] = &server::kickFun;
}