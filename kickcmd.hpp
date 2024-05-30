#ifndef KICKCMD_HPP
#define KICKCMD_HPP

#include"Channel.hpp"

class Channel;
class KickCmd{
    public:
    std::string client_name;
    int kicked_fd;
    KickCmd();
    ~KickCmd();
        void kick(std::string av[2], Channel &ch,int fd);
       
};
 void send_response(int fd, std::string response);
#endif