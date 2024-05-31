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




void Server::sendToClient(int fd, const std::string& message) {
    send(fd, message.c_str(), message.size(), 0);
}


void Server::parseClientInput(int fd, const std::string& data, Channel ch) {
    std::istringstream stream(data);  // Create an input string stream from the data string
    std::string line;
    while (std::getline(stream, line)) {  // Read each line from the stream
        std::istringstream linestream(line);  // Create an input string stream from the line
        std::string command;
        linestream >> command;  // Read the command from the line
if(command=="quit")
{
    std::cout  << "Client <" << fd << "> Disconnected" << std::endl;
	
		close(fd);
}
        for (size_t i = 0; i < clients.size(); ++i) {
             Client& client = clients[i];
            if (client.getFd() == fd) {
                if (command == "CAP")
                {
                    std::string pass = "Please enter your password:\r\n";
                    send(fd, pass.c_str(), pass.size(), 0);
                }
                if (!client.hasPasswordReceived() && command == "PASS" && count ==0) {
                    std::string pass;
                    linestream >> pass;  // Read the password
                    client.setPassword(pass);
                    std::cout<<"my password: "<<pass<<std::endl;
                    count =1;

                    // Prompt for nickname after receiving password
                    std::string nicknamePrompt = "Please enter your nickname:\r\n";
                    send(fd, nicknamePrompt.c_str(), nicknamePrompt.size(), 0);
                    client.setPasswordReceived(true);
                } else if (client.hasPasswordReceived() && !client.hasNicknameReceived() && command == "NICK"&&count ==1) {
                    std::string nick;
                    linestream >> nick;  // Read the nickname
                    client.setNickname(nick);
                   std::cout<<"my nick: "<<nick<<std::endl;

                    // Prompt for username after receiving nickname
                    std::string usernamePrompt = "Please enter your username:\r\n";
                    send(fd, usernamePrompt.c_str(), usernamePrompt.size(), 0);
                    count=2;
                    client.setNicknameReceived(true);
                } else if (client.hasPasswordReceived() && client.hasNicknameReceived() && !client.hasUsernameReceived() && command == "USER"&&count ==2) {
                    std::string user, mode, unused, realname;
             
                    client.setUsername(user);
                    std::cout<<"my User: "<<user<<std::endl;

                    // Client setup is complete, you can now proceed with further handling
                    std::string welcomeMessage = ":myserver 001 " + client.getNickname()  + " :Welcome to the IRC server\r\n";
                    std::string yourHostMsg = ":myserver 002 " + client.getNickname() + " :Your host is myserver\r\n";
                    std::string createdMsg = ":myserver 003 " + client.getNickname()+ " :This server was created just now\r\n";
                    std::string myInfoMsg = ":myserver 004 " + client.getNickname()+ " myserver v1.0 i\r\n";
  
                    send(fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
                    send(fd, yourHostMsg.c_str(), welcomeMessage.size(), 0);
                    send(fd, createdMsg.c_str(), welcomeMessage.size(), 0);
                    send(fd, myInfoMsg.c_str(), welcomeMessage.size(), 0);
                    client.setUsernameReceived(true);

                }
                }
                break;
            }
        }}

//for the hexchat
// void Server::parseClientInput(int fd, const std::string& data) {
//     std::istringstream stream(data);
//     std::string line;
//     while (std::getline(stream, line)) {
//         std::istringstream linestream(line);
//         std::string command;
//         linestream >> command;

//         if (command == "PASS") {
//             std::string pass;
//             linestream >> pass;

//             // Find the client and set the password
//             for (auto& client : clients) {
//                 if (client.getFd() == fd) {
//                     client.setPassword(pass);
//                     std::cout<<"our password "<<pass<<std::endl;
//                     break;
//                 }
//             }
//         } else if (command == "NICK") {
//             std::string nick;
//             linestream >> nick;

//             // Find the client and set the nickname
//             for (auto& client : clients) {
//                 if (client.getFd() == fd) {
//                     client.setNickname(nick);
//                     std::cout<<"our Nickname "<<nick<<std::endl;
//                     break;
//                 }
//             }
//         } else if (command == "USER") {
//             std::string user, mode, unused, realname;
//             linestream >> user >> mode >> unused;
//             std::getline(linestream, realname);
//             if (!realname.empty() && realname[0] == ':') {
//                 realname = realname.substr(1); // Remove the leading colon
//             }

//             // Find the client and set the username and realname
//             for (auto& client : clients) {
//                 if (client.getFd() == fd) {
//                     client.setUsername(user);
//                     std::cout<<"our Username "<<user<<std::endl;
//                    // client.setRealname(realname);
//                     break;
//                 }
//             }
//         }
//     }
// }