#include"Server.hpp"


bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true; //-> set the static boolean to true to stop the server
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
void Server::CloseFds(){
	for(size_t i = 0; i < clients.size(); i++){ //-> close all the clients
		std::cout << RED << "Client <" << clients[i].getFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].getFd());
	}
	if (fd_Server != -1){ //-> close the server socket
		std::cout << RED << "Server <" << fd_Server << "> Disconnected" << WHI << std::endl;
		close(fd_Server);
	}
}

void Server::AcceptNewConnetinClient(){
    Client new_client;
    struct sockaddr_in client_add;
    struct pollfd clientPoll;
    socklen_t length = sizeof(client_add);
    int accept_cl = accept(fd_Server,(sockaddr *)&(client_add),&length);
        if (accept_cl == -1)
    {std::cout <<RED<< "client cannot connect" << std::endl; return;}
    clientPoll.fd=accept_cl;
    clientPoll.events=POLLIN;
    clientPoll.revents=0;
    new_client.SetFd(accept_cl); //-> set the client file descriptor
    new_client.SetIppAdd(inet_ntoa((client_add.sin_addr))); //-> convert the ip address to string and set it
    parseClientInput(new_client ,accept_cl); // -> parse the client input
    clients.push_back(new_client); //-> add the client to the vector of clients
    fds.push_back(clientPoll); //-> add the client socket to the pollfd
    std::cout <<GRE<<"client connected seccefully" <<WHI<< std::endl;
    // Send IRC welcome messages  
}

void Server::ReceiveNewData(int fd)
{
    //this is the buff thata we wiil store our data received in
	char buff[1024]; 
    Client c;

    //clear the buffer
	memset(buff, 0, sizeof(buff)); 
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0); 
    //if the client disconnected
	if(bytes <= 0){ 
		std::cout  <<RED<< "Client <" << fd << "> Disconnected" << std::endl;
	
		close(fd);
	}
//if not print the data received
	else{ 
		buff[bytes] = '\0';
        // static ch nwebuff += buff;
        // if ( != std::npos)
        std::string data(buff);
        std::cout << "buf : " << data << "fin" <<std::endl;
        ParseCmd(data, *this, fd);
		std::cout <<ORANGE<< "Client <" <<RESET<< fd_Server <<ORANGE<< "> Data: "  << buff<<RESET;
	}
}

void Server::be_ready_for_connection()
{
    struct sockaddr_in add;
    struct pollfd NewPoll;
    add.sin_family=AF_INET;
    add.sin_port=htons(this->port);
   this->fd_Server = socket(AF_INET,SOCK_STREAM,0);
//    if( this->fd_Server==-1)
//     throw(std::runtime_error("failed to create socket"));

    //active non_blocking socket
    int en =1;
    if(setsockopt( this->fd_Server, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
        throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
    if (fcntl( this->fd_Server, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
        throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
      // Bind socket to address and port
    if (bind( this->fd_Server, (struct sockaddr *)&add, sizeof(add)) < 0) {
        std::cerr <<RED<< "Bind failed" << RESET<<std::endl;
        exit(1);
    }    
    // Listen for incoming connections
    if (listen( this->fd_Server, SOMAXCONN) == -1) {
        std::cerr <<RED<< "Listen failed" << RESET<<std::endl;
        exit(1);
    }
    NewPoll.fd =  this->fd_Server; //-> add the server socket to the pollfd
    NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
    NewPoll.revents = 0; //-> set the revents to 0
    fds.push_back(NewPoll); //-> add the server socket to the pollfd
    std::cout<<GRE<<"server <"<< this->fd_Server<<">"<<GRE<<" and ready for receiving data"<<RESET<<std::endl;
    std::cout<<YEL<<"Waiting to accept conection"<<std::endl;
    while(Server::Signal == false&& Server::Signal == false)
    {
        if(poll(&fds[0],fds.size(),-1) ==-1 && Server::Signal == false)
            throw (std::runtime_error("our pool function failed"));
        for(size_t i = 0;i < fds.size();i++)
        {
            if(fds[i].revents & POLLIN)
            {
                if(fds[i].fd ==  this->fd_Server)
                {
                    std::cout<<GRE<<"accepting new client"<<WHI<<std::endl;
                    AcceptNewConnetinClient();
                }
                else
                    ReceiveNewData(fds[i].fd);
            }
        }
    }
    CloseFds();
}
