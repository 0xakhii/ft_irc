#ifndef KICKCMD_HPP
#define KICKCMD_HPP

#include"Channel.hpp"

class Channel;
class KickCmd{
    public:
        void kick(std::string av[2], Channel &ch);
};

#endif