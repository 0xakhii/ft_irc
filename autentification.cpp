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
#include"Server.hpp"
#include"Channel.hpp"
#include <cstdlib>

void Server::parseClientInput(Client& new_client, int fd) {
    count = 0;
    while(1){
        char buff[1024]; 
        memset(buff, 0, sizeof(buff)); 
        ssize_t bytes = recv(new_client.getFd() , buff, sizeof(buff) - 1 , 0);
        if (bytes <= 0){
            cout << "Clients Disconnected\n";
            return ;
        }
        else{
            buff[bytes] = '\0';
            std::istringstream stream(buff);  // Create an input string stream from the data string
            std::string line;
            while (std::getline(stream, line)) {  // Read each line from the stream
                std::istringstream linestream(line);  // Create an input string stream from the line
                std::string command;
                linestream >> command;
                if(command=="quit")
                {
                    std::cout  <<RED<< "Client <" << fd << "> Disconnected" << std::endl;
                    close(fd);
                }
                if (command == "CAP")
                {
                    std::string passe = colorCode("Please enter your password:\r\n",3);
                    send(fd, passe.c_str(), passe.size(), 0);
                }
                if (!new_client.hasPasswordReceived() && command == "PASS" && count ==0) {
                    std::string passe;
                    linestream >> passe;  // Read the password
                    if(passe!=pass)
                    {
                        std::string pass_err=colorCode(ERR_PASSWDMISMATCH(),5);
                        std::string passe = colorCode("Please enter your password \r\n",3);
                        send(fd,pass_err.c_str(),pass_err.size(),0);
                        send(fd, passe.c_str(), passe.size(), 0);
                        continue;
                    }
                    new_client.setPassword(passe);
                    count =1;
                    new_client.setPasswordReceived(true);
                    // Prompt for nickname after receiving password
                    std::string nicknamePrompt = colorCode("Please enter your nickname:\r\n",3);
                    send(fd, nicknamePrompt.c_str(), nicknamePrompt.size(), 0);
                } else if (new_client.hasPasswordReceived() && !new_client.hasNicknameReceived() && command == "NICK"&&count ==1) {
                    std::string nick;
                    linestream >> nick;  // Read the nickname
                    if(!prsNickname(nick,fd))
                        continue;
                    new_client.setNickname(nick);
                    new_client.setNicknameReceived(true);
                    // Prompt for username after receiving nickname
                    std::string usernamePrompt = colorCode("Please enter your username:\r\n",3);
                    send(fd, usernamePrompt.c_str(), usernamePrompt.size(), 0);
                    count=2;
                } else if (new_client.hasPasswordReceived() && new_client.hasNicknameReceived() && !new_client.hasUsernameReceived() && command == "USER"&&count ==2) {
                    std::string user, mode, unused, realname;
                    linestream >> user;
                    new_client.setUsername(user);
                    new_client.setUsernameReceived(true);
                    // Client setup is complete, you can now proceed with further handling
                }
            }
        }
        if (new_client.hasNicknameReceived()
            && new_client.hasPasswordReceived() && new_client.hasUsernameReceived()){
            send_welcome_message(fd,new_client);
            break;
        }
    }
}

void Server::send_welcome_message(int fd,Client client)
{
    std::string welcomeMessage =colorCode(":myserver 001 " + client.getNickname()  + " :Welcome to the IRC server\r\n",6);
    std::string yourHostMsg = colorCode(":myserver 002 " + client.getNickname() + " :Your host is myserver\r\n",6);
    std::string createdMsg = colorCode(":myserver 003 " + client.getNickname()+ " :This server was created just now\r\n",6);
    std::string myInfoMsg = colorCode(":myserver 004 " + client.getNickname()+ " myserver v1.0 i\r\n",6);
  
    send(fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
    send(fd, yourHostMsg.c_str(), yourHostMsg.size(), 0);
    send(fd, createdMsg.c_str(), createdMsg.size(), 0);
    send(fd, myInfoMsg.c_str(), myInfoMsg.size(), 0);
}