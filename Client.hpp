#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "Server.hpp"

class Client{
    int fd;
    
    std::string ip_add;
    std::string Nickname;
    std::string Username;
    std::string Password;
    std::string realName;
    bool hasPassword;
    bool hasNickname;
    bool hasUsername;
    public:
    int count;
    Client(){
        count =0;

    };
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
     std::string getRealname() { return realName; }
     void setRealname(std::string real) { realName = real; }
    void setUsername(std::string user) { Username = user; }
    std::string getPassword() { return Password; }
    void setPassword(std::string pass) { Password = pass; }
    bool hasPasswordReceived() { return hasPassword; }
    bool hasNicknameReceived() { return hasNickname; }
    bool hasUsernameReceived() { return hasUsername; }
    void setPasswordReceived(bool hasPassword) { this->hasPassword = hasPassword; }
    void setNicknameReceived(bool hasNickname) { this->hasNickname = hasNickname; }
    void setUsernameReceived(bool hasUsername) { this->hasUsername = hasUsername; }

};
#endif