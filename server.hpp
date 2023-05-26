/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 16:44:55 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/26 03:10:45 by jbettini         ###   ########.fr       */
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
                void        modeFun(Client & client, std::vector<std::string> clientInput);
                void        pingFun(Client & client, std::vector<std::string> clientInput);
                void        nickFun(Client & client, std::vector<std::string> clientInput);
                void        userFun(Client & client, std::vector<std::string> clientInput);
                void        passFun(Client & client, std::vector<std::string> clientInput);
                void        capFun(Client & client, std::vector<std::string> clientInput);
                
                
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

void                        add_back(std::vector<Client> & vector, const Client & objet);
std::vector<std::string>    splitBuffer(char* buffer, const std::string& delimiters);
std::vector<std::string>    removeWhitespace(std::vector<std::string>& strings);
std::vector<std::string>    makeVecKey(std::vector<std::string> strings, std::string toFind);
std::string                 getMsg(std::string first, std::string last, Client & client);
void                        printVecStr(std::vector<std::string> strings);
int                         findString(std::vector<std::string> strings, std::string toFind);
void                        printClient(std::vector<Client> strings);

#endif