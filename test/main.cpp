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

int main(int ac, char **av) {
    std::string test = "*!*tagrand mere ++ + @*.0.01.ip";
    if (checkFormat(test))
        std::cout << "format irssi" << std::endl;
    else
        std::cout << "format nc" << std::endl;
    std::cout << "username = ->" << extractUsernameModeFormat(test) << "<-" << std::endl;
    return 0;
}