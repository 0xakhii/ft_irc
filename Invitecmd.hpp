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
};
