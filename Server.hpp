#pragma once

#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()


class Client{
    int fd;
    std::string ip_add;
    public:
        Client(){};
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
};

class Server{
    private:
    std::vector<Client> clients;
    int port;
    int fd_Server;
    //static bool signal;
    std::vector<struct pollfd>fds;
    public:
        Server(){
            fd_Server = -1;
        }
    int be_ready_for_connection();
    void AcceptNewConnetinClient();
    void ClearClients(int fd);
};