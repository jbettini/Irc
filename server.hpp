/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 16:44:55 by jbettini          #+#    #+#             */
/*   Updated: 2023/06/01 23:47:10 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef S_HPP
    #define S_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"

#define NC 0
#define IRSSI 1

    class   ClientNotFoundException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from  Get Client, client not find!");
        }
    };

    class   ChannelNotFoundException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from  Get Channel, channel not find!");
        }
    };

    class   serverException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from server !");
        }
    };
    class   pollException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from poll !");
        }
    };
    class   socketException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from  socket !");
        }
    };

    class   bindException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from  bind !");
        }
    }; 

    class   listenException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from listen !");
        }
    };

    class   acceptException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from accept !");
        }
    };

    class   recvException : public std::exception {
        virtual const char* what() const throw() {
            return ("Error : from recv !");
        }
    };


    // class   Exception : public std::exception {
    //     virtual const char* what() const throw() {
    //         return ("Error : from  !");
    //     }
    // };

class   server {

    public :
                server(void);
                server(server & rhs);
                ~server(void);
                server(int port, std::string pswd);
                void    initFunLst(void);


                void        run(void);
                void        init_socket(void);
                void        addChannel(std::string name);
                void        printChannel(void);
                void        execInput(std::vector<std::string> clientInput, Client & client);
                void        displayClient(std::string   msg, Client client);
                void        defineClientUsername(Client & client, std::vector<std::string> clientInput);
                void        disconnectClient(Client & client);
                void        welcomeMsg(Client & client);
                void        removeUserInChannel(Client & client);
                void        welcomeToChannel(Client & client, std::string channelName);
                void        sendToAllUserInChannel(std::string channelName, std::string msg, Client & client);
                void        sendToAllClientChannel(Client & client, std::vector<std::string> clientInput, int check);
                void        updateChannelListNick(std::vector<Channel> & channelList, std::string currentNick, std::string newNick);
                void        modeFun(Client & client, std::vector<std::string> clientInput);
                void        pingFun(Client & client, std::vector<std::string> clientInput);
                void        nickFun(Client & client, std::vector<std::string> clientInput);
                void        userFun(Client & client, std::vector<std::string> clientInput);
                void        passFun(Client & client, std::vector<std::string> clientInput);
                void        capFun(Client & client, std::vector<std::string> clientInput);
                void        joinFun(Client & client, std::vector<std::string> clientInput);
                void        banFun(Client & client, std::vector<std::string> clientInput);
                void        quitFun(Client & client, std::vector<std::string> clientInput);
                void        sendChannelMessage(Client & client, std::vector<std::string> clientInput);
                void        privmsgFun(Client & client, std::vector<std::string> clientInput);
                void        opFun(Client & client, std::vector<std::string> clientInput);
                void        partFun(Client & client, std::vector<std::string> clientInput);
                void        noticeFun(Client & client, std::vector<std::string> clientInput);
                void        whoFun(Client & client, std::vector<std::string> clientInput);
                void        listFun(Client & client, std::vector<std::string> clientInput);
                void        topicFun(Client & client, std::vector<std::string> clientInput);
                void        inviteFun(Client & client, std::vector<std::string> clientInput);
                void        setTopicRestrictionFun(Client & client, std::vector<std::string> clientInput);
                void        handleModeiFun(Client & client, std::vector<std::string> clientInput);
                void        kickFun(Client & client, std::vector<std::string> clientInput);
                void        setPasswordRestrictionFun(Client & client, std::vector<std::string> clientInput);
                void        setClientLimitRestrictionFun(Client & client, std::vector<std::string> clientInput);
                void        sendToAllUser(std::string channelName, std::string msg);
                void        modeLstFun(Client & client, std::vector<std::string> clientInput);
                void        banLstFun(Client & client, std::vector<std::string> clientInput);
                
                std::string getAllUsersChannel(Channel & channel);
                
                server  &   operator=(server & rhs);

                int         getPort(void) {
                    return (this->_port);
                }
                std::string getPassword(void) {
                    return (this->_password);
                }
                Client    &  getClient(int newClient){
                    for (size_t i = 0; i != this->_ClientList.size(); i++) {
                        if (this->_ClientList[i].getCS() == newClient)
                            return (this->_ClientList[i]);
                    }
                    throw ClientNotFoundException();
                    return (this->_ClientList[0]);
                }

                Client    &  getClientWithNick(std::string nick) {
                    for (size_t i = 0; i != this->_ClientList.size(); i++) {
                        if (this->_ClientList[i].getNick() == nick)
                            return (this->_ClientList[i]);
                    }
                    return (this->_ClientList[0]);
                }

                Client    &  getClientWithUser(std::string user) {
                    for (size_t i = 0; i != this->_ClientList.size(); i++) {
                        if (this->_ClientList[i].getUsername() == user)
                            return (this->_ClientList[i]);
                    }
                    return (this->_ClientList[0]);
                }

                Channel    &  getChannel(std::string channelName){
                    for (size_t i = 0; i != this->_ChannelList.size(); i++) {
                        if (this->_ChannelList[i].getChannelName() == channelName)
                            return (this->_ChannelList[i]);
                    }
                    return (this->_ChannelList[0]);
                }

                bool            channelExist(std::string toFind) {
                    for (std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++)
                        if ((*it).getChannelName() == toFind) {
                            return true;
                        }
                    return false;
                }

                bool            checkNickExist(std::string toFind) {
                    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
                        if ((*it).getNick() == toFind)
                            return true;
                    return false;
                }

                bool            checkUsernameExist(std::string toFind) {
                    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
                        if ((*it).getUsername() == toFind)
                            return true;
                    return false;
                }
                
                std::vector<Channel> & getChannelList(void){
                   return this->_ChannelList;
                }
                
                std::string            getUsernameByNick(std::string nick) {
                    for (std::vector<Client>::iterator it = this->_ClientList.begin(); it != this->_ClientList.end(); it++)
                        if ((*it).getNick() == nick)
                            return it->getUsername();
                    return nick;
                }

                void                   removeChannelToChannelList(std::string channelName) {
                    for (std::vector<Channel>::iterator it = this->_ChannelList.begin(); it != this->_ChannelList.end(); it++)
                        if ((*it).getChannelName() == channelName) {
                            this->_ChannelList.erase(it);
                            break ;
                        }
                            
                }

                int     MAX_CLIENTS;
                int     MAX_BUFFER_SIZE;

    private :
                int                         _socket;
                int                         _port;
                std::string                 _password; 
                struct  sockaddr_in         _serverAddress;
                std::vector<Client>         _ClientList;
                std::vector<Channel>        _ChannelList;
                struct  pollfd              *_ClientFd;
                typedef void (server::*Fun)(Client &, std::vector<std::string>);
                std::map<std::string, Fun>  _FunLst;
                void static   *ping(void * args);

};

std::vector<std::string>  & removeWhitespace(std::vector<std::string>& strings);
std::vector<std::string>    splitBuffer(char* buffer, std::string delimiters);
std::vector<std::string>    splitString(const std::string& input, const std::string& delimiters); 
std::vector<std::string>    makeVecKey(std::vector<std::string> strings, std::string toFind);
std::vector<std::string>    removeFirstCharacterIfColon(std::vector<std::string>& strings);
std::vector<std::string>    removeFirstCharacterIfColonIdx(std::vector<std::string>& strings, size_t idx);
std::vector<std::string>    removeDelimiterStrings(std::vector<std::string>& strings, std::string& delimiters);
std::string                 getMode(std::vector<std::string> clientInput);
std::string                 getMsg(std::string first, std::string last, Client & client);
std::string                 extractUsernameModeFormat(const std::string& str);
std::string                 catVecStr(std::vector<std::string> toCat, int idx);
std::string                 removeHash(const std::string& str);
void                        printVecStr(std::vector<std::string> strings);
void                        printClient(std::vector<Client> strings);
void                        updateVectorStr(std::vector<std::string> & toUpdate, std::string oldStr, std::string newStr);
void                        printfChannel(Channel channel);
void                        add_back(std::vector<Client> & vector, const Client & objet);
bool                        checkNonAlphanumeric(const std::string& str);
bool                        checkNameChannel(const std::string& str);
bool                        checkFunWelcome(std::string fun);
bool                        checkFormat(const std::string& str);
int                         findString(std::vector<std::string> strings, std::string toFind);

#endif