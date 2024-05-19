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

using namespace std;

class Client{
    int fd;
    string ip_add;
    public:
        Client(){};
        int GetFd()
        {
            return fd;
        };
        void SetFd(int Fd)
        {
            fd = Fd;
        }
        void SetIppAdd(string ip)
        {
            ip_add = ip;

        }
};

class Server{
    private:
    vector<Client> clients;
    int port;
    int fd_Server;
    //static bool signal;
    vector<struct pollfd>fds;
    public:
        Server(){
            fd_Server = -1;
        }
    int be_ready_for_connection();
    void AcceptNewConnetinClient();
    void ClearClients(int fd);
};
