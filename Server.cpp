#include "Server.hpp"



int Server::be_ready_for_connection()
{
    struct sockaddr_in add;
    struct pollfd NewPoll;
    add.sin_family=AF_INET;
    this->port= 4444;
    add.sin_port=htons(this->port);
    //creating sockets 
    
   int fd = socket(AF_INET,SOCK_STREAM,0);
   if(fd==-1){
    throw(std::runtime_error("failed to create socket"));
   }

    //active non_blocking socket
    int en =1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
        throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
        throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
      // Bind socket to address and port
    if (bind(fd, (struct sockaddr *)&add, sizeof(add)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }   
    // Listen for incoming connections
    if (listen(fd, SOMAXCONN) == -1) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    NewPoll.fd = fd; //-> add the server socket to the pollfd
    NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
    NewPoll.revents = 0; //-> set the revents to 0
    fds.push_back(NewPoll); //-> add the server socket to the pollfd
    std::cout<<"server"<<fd_Server<<" connected and ready for receiving data"<<std::endl;
    std::cout<<"Waiting to accept conection"<<std::endl;
    while(1)
    {
        if(poll(&fds[0],fds.size(),-1) ==-1)
            throw (std::runtime_error("our pool function failed"));
        for(size_t i = 0;i < fds.size();i++)
        {
            if(fds[i].revents & POLLIN)
            {
                if(fds[i].fd == fd_Server)
                {
                    std::cout<<"accepting new client"<<std::endl;
                    //AcceptNewClient();
                }
                else{
                    std::cout<<"reicinvind a new data from a registred client";
                    //ReceiveNewData();
                }
            }
        }
    }
    close(fd_Server);
}


 void Server::AcceptNewConnetinClient(){
    
 }
















void Server::ClearClients(int fd){ //-> clear the clients
 for(size_t i = 0; i < fds.size(); i++){ //-> remove the client from the pollfd
  if (fds[i].fd == fd)
   {fds.erase(fds.begin() + i); break;}
 }
 for(size_t i = 0; i < clients.size(); i++){ //-> remove the client from the vector of clients
  if (clients[i].getFd() == fd)
   {clients.erase(clients.begin() + i); break;}
 }

}
int main()
{

}