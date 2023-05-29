/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:29:12 by jbettini          #+#    #+#             */
/*   Updated: 2023/05/28 23:00:52 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"


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

int findString(std::vector<std::string> strings, std::string toFind) {
    for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end();it++)
        if (*it == toFind)
            return (1);
    return (0);
}

std::string retAfterFind(std::vector<std::string> strings, std::string toFind) {
    for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end();it++)
        if (*it == toFind)
            return (*(++it));
      
    return (toFind);
}


std::vector<std::string> makeVecKey(std::vector<std::string> strings, std::string toFind) {
    if (findString(strings, toFind)) {
        std::vector<std::string>    ret;
        ret.push_back(toFind);
        std::string after = retAfterFind(strings, toFind);
        if (after != toFind)
            ret.push_back(after);
        else
            return strings;
        return ret;
    }
    return strings;
}

std::vector<std::string> & removeWhitespace(std::vector<std::string>& strings) {
    for (size_t i = 0; i < strings.size(); ++i) {
        std::string& str = strings[i];
        std::string::iterator iter = str.begin();
        while (iter != str.end()) {
            if (*iter == ' ' || (*iter >= 9 && *iter <= 13)) {
                iter = str.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    return strings;
}

std::vector<std::string> removeDelimiterStrings(std::vector<std::string>& strings, std::string& delimiters) {
    std::vector<std::string> result;
    for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end(); ++it) {
        bool containsNonDelimiterChar = false;
        for (std::string::iterator iter = it->begin(); iter != it->end(); ++iter)
            if (delimiters.find(*iter) == std::string::npos) {
                containsNonDelimiterChar = true;
                break;
            }
        if (containsNonDelimiterChar)
            result.push_back(*it);
    }
    return result;
}

std::vector<std::string> splitBuffer(char* buffer,std::string delimiters) {
    std::vector<std::string> splited;
    std::string tmp(buffer);
    size_t pos = 0;
    
    while ((pos = tmp.find_first_of(delimiters)) != std::string::npos) {
        std::string token = tmp.substr(0, pos);
        tmp.erase(0, pos + 1);
        splited.push_back(token);
    }
    
    splited.push_back(tmp);
    splited = removeDelimiterStrings(removeWhitespace(splited), delimiters);
    return splited;
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

bool checkNonAlphanumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        if (!std::isalnum(*it) && (*it) != '_')
            return true;
    return false;
}

bool    checkNameChannel(const std::string& str) {
    if (str.empty() || str[0] != '#')
        return false;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        if (!std::isprint(*it))
            return false;
    return true;
}

bool    checkFunWelcome(std::string fun) {
    if (fun == "NICK" || fun == "USER" || fun == "PASS" || fun == "CAP")
        return true;
    return false;
}

std::string catVecStr(std::vector<std::string> toCat, int idx) {
    std::string result;
    std::vector<std::string>::iterator it;
    for (it = toCat.begin() + --idx; it != toCat.end(); ++it) {
        result += *it;
        if (it + 1 != toCat.end())
            result += " ";
    }
    std::cout << "MSG : " << result << std::endl;
    return result;
}

std::string getMode(std::vector<std::string> clientInput) {
    for (std::vector<std::string>::iterator it = clientInput.begin(); it != clientInput.end(); it++) {
        if (*it == "+i")
            return ("+i");
        else if (*it == "-i")
            return ("-i");
        else if (*it == "-b")
            return ("-b");
        else if (*it == "+b")
            return ("+b");
        else if (*it == "+o")
            return ("+o");
        else if (*it == "-o")
            return ("-o");
    }
    return ("");
}

bool checkFormat(const std::string& str) {
    if (str.length() < 14)
        return false;
    if (str.substr(0, 3) != "*!*")
        return false;
    if (str.substr(str.length() - 11) != "@*.0.0.1.ip")
        return false;
    return true;
}

std::string extractUsernameModeFormat(const std::string& str) {
    std::string username;
    std::size_t start = str.find("*!*") + 3; // position du premier caractère de l'username
    std::size_t end = str.find("@", start); // position du caractère '@' après l'username

    if (start != std::string::npos && end != std::string::npos)
        username = str.substr(start, end - start);
    return username;
}

void    updateVectorStr(std::vector<std::string> & toUpdate, std::string oldStr, std::string newStr) {
    for (std::vector<std::string>::iterator it = toUpdate.begin(); it != toUpdate.end(); it++)
        if (*it == oldStr)
            *it = newStr;
}