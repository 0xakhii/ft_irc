#ifndef SERVER_HPP
#define SERVER_HPP

#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color
#define ORANGE "\033[38;2;255;165;0m" //->orange

#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <cstring>//for memset
#include <signal.h> //-> for signal()
#include <sstream>
#include "Client.hpp"
#include <cstdlib>
#include "headers_irc.hpp"
#include "Channel.hpp"

class Channel;

class Server{
    private:
        int fd_Server;
        std::vector<struct pollfd>fds;
    public:
        static bool Signal;
        Channel ch;
        std::vector<Client> clients;
        int port;
        int count;
        std::string pass;
        Server(){
            fd_Server = -1;
            count = 0;
        }
        void be_ready_for_connection();
        void AcceptNewConnetinClient();
        void ReceiveNewData(int fd);
        void ClearClients(int fd);
        void parseClientInput(int fd, const std::string& data);
        bool prsNickname(std::string nickname,int fd);
        bool validateNickname(std::string nickname);
        std::string colorCode(const std::string& message, int color);
        void send_welcome_message(int fd,Client client);
        static void SignalHandler(int signum);
        void CloseFds();
        bool parsUSer(int i,std::string unusedInt,std::string unusedChar,std::string command,int fd,std::string realname,std::string username);
};

void	ParseCmd(std::string cmd, Server& serv, int fd);

#endif