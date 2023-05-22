/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoudin <mgoudin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/10 17:03:34 by mgoudin           #+#    #+#             */
/*   Updated: 2023/05/17 19:55:12 by mgoudin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

class Client {
    private:
        std::string nickName;
        std::string userName;
        int         socketId;
        int         fdId;

    public:
        explicit Client(const int socketId, const int fdId) : socketId(socketId), fdId(fdId) {};

        void setNickName(std::string nickname)
        {
            if (nickname.size() < 2)
                return;
            nickName = nickname;
        }

        void setUserName(std::string username)
        {
            if (username.size() < 2)
                return;
            userName = username;
        }

        std::string getNickName() {return nickName;}
        std::string getUserName() {return userName;}
        int         getSocketId() {return socketId;}
        int         getFdId() {return fdId;}
};
