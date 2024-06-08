#ifndef KICKCMD_HPP
#define KICKCMD_HPP

#include"Channel.hpp"

class Channel;
class KickCmd{
    public:
    std::string client_name;
    int kicked_fd;
    std::vector<std::string> args;
    KickCmd();
    ~KickCmd();
    void kick(std::string full_args, Channel &ch,int fd);
    std::string notenoughparams();
    std::string nosuchchannel(std::string channel_name);
    std::string nosuchuser(std::string kicked_nick);
    std::string notadmin(std::string source_nick,std::string channel_name);
    std::string kickyourself(std::string source_nick);
       
};
 void send_response(int fd, std::string response);
void sendresp_all(int fd,std::string kicked_nick,std::string source_nick,std::string channel_name,std::string comment);
 int findUserFdByUsername(const std::map<std::string, int>& userMap,  std::string& username);
 void splitkickargs(std::string& str);
#endif