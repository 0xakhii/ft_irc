#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <cstring>
#include <algorithm>
#include <exception>
#include "Channel.hpp"

#define ERR "\033[1;31mError:\033[0;0m\n\033[1m"
#define RESET "\033[0;0m"

using namespace std;
class Client{
    
    public:
    int fd;
    std::string ip_add;
    string  username;
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
        void setUser(string _username){
            username = _username;
        }
        string getUser(){
            return username;
        }
};

class Channel;
class Server{
    private:
    
    
    int fd_Server;
    //static bool signal;
    std::vector<struct pollfd>fds;
    public:
    std::vector<Client> clients;
    Channel ch;
    int port;
    std::string pass;
        Server(){
            fd_Server = -1;
        }
    int be_ready_for_connection();
    void AcceptNewConnetinClient();
    void ReceiveNewData(int fd, Channel &ch);
    void ClearClients(int fd);
};
void	ParseCmd(string cmd, Channel &ch, Server serv, int fd);
void	createChannel(string arg, Channel &ch, string username, int fd);

#endif