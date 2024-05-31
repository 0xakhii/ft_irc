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
#include <cstring>//for memset
#include<sstream>
#include"Client.hpp"
#include <cstdlib>
#include <cstring>//for memset
#include<sstream>
#include"Client.hpp"
#include <cstdlib>
#include "Channel.hpp"

class Channel;
class Server{


    private:
    
    
    int fd_Server;
    //static bool signal;
    std::vector<struct pollfd>fds;
    public:
    Channel ch;
    std::vector<Client> clients;
    int port;
    int count;
    std::string pass;
        Server(){
            fd_Server = -1;
            count = 0;
        }
    int be_ready_for_connection();
    void AcceptNewConnetinClient();
    void ReceiveNewData(int fd, Channel ch);
    void ClearClients(int fd);
    void sendToClient(int fd, const std::string& message);
    void parseClientInput(Client& new_client);
};
void	ParseCmd(std::string cmd, Channel &ch, Server serv, int fd);

#endif