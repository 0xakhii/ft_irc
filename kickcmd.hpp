#ifndef KICKCMD_HPP
#define KICKCMD_HPP

#include"Channel.hpp"

class Channel;
class KickCmd{
    public:
        KickCmd();
        ~KickCmd();
        void kick(std::string av[2], Channel &ch);
};

#endif