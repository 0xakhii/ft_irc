#pragma once

#include "Channel.hpp"
#include "Server.hpp"
#include "kickcmd.hpp"

class Invitecmd{
public:
    std::string client_name;
    int invited_fd;
    Invitecmd();
    ~Invitecmd();
    void invite(std::string av[2], Channel &ch,int fd);
    std::string notenoughparams();
    std::string nosuchchannel(std::string channel_name);
    std::string nosuchuser(std::string invited_nick);
    std::string notadmin(std::string source_nick,std::string channel_name);
    std::string Rpl_inviting(std::string source_nick,std::string channel_name,std::string invited_nick);
    std::string useronchannel(std::string invited_nick,std::string channel_name);
};