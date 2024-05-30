#pragma once

#include "Channel.hpp"
#include "Server.hpp"
#include "kickcmd.hpp"

class Invitecmd{
public:
    Invitecmd();
    ~Invitecmd();
    void invite(std::string av[2], Channel &ch,int fd);
};