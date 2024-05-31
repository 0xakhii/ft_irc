#pragma once
#include "Server.hpp"

class Client{
    int fd;
    std::string ip_add;
    std::string Nickname;
    std::string Username;
    std::string Password;
      bool hasPassword;
    bool hasNickname;
    bool hasUsername;
    public:
        int getFd()
        {
            return fd;
        };
        void SetFd(int Fd)
        {
            fd = Fd;
        }
        void SetIppAdd(std::string ip)
        {
            ip_add = ip;

        }
       
    std::string getNickname() { return Nickname; }
    void setNickname(std::string nick) { Nickname = nick; }
    std::string getUsername() { return Username; }
    void setUsername(std::string user) { Username = user; }
    std::string getPassword() { return Password; }
    void setPassword(std::string pass) { Password = pass; }
    bool hasPasswordReceived() { return hasPassword; }
    void setPasswordReceived(bool hasPassword) { this->hasPassword = hasPassword; }
    void setNicknameReceived(bool hasNickname) { this->hasNickname = hasNickname; }
    void setUsernameReceived(bool hasUsername) { this->hasUsername = hasUsername; }
    bool hasNicknameReceived() { return hasNickname; }
    bool hasUsernameReceived() { return hasUsername; }
};