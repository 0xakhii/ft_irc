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


void Server::parseClientInput(Client& new_client) {
    count = 0;
    while(1){
        char buff[1024]; 
        memset(buff, 0, sizeof(buff)); 
        ssize_t bytes = recv(new_client.getFd() , buff, sizeof(buff) - 1 , 0);
        if (bytes <= 0)
            cout << "Clients Disconnected\n";
        else{
            buff[bytes] = '\0';
            std::istringstream stream(buff);  // Create an input string stream from the data string
            std::string line;
            while (std::getline(stream, line)) {  // Read each line from the stream
                std::istringstream linestream(line);  // Create an input string stream from the line
                std::string command;
                linestream >> command;  // Read the command from the line
                if(command=="quit")
                {
                    std::cout  << "Clients <" << new_client.getFd()  << "> Disconnected" << std::endl;
                    
                        close(new_client.getFd() );
                }
                if (command == "CAP")
                {
                    std::string pass = "Please enter your password:\r\n";
                    send(new_client.getFd() , pass.c_str(), pass.size(), 0);
                }
                if (!new_client.hasPasswordReceived() && command == "PASS" && count ==0) {
                    std::string pass;
                    linestream >> pass;  // Read the password
                    new_client.setPassword(pass);
                    std::cout<<"my password: "<<pass<<std::endl;
                    count =1;

                    // Prompt for nickname after receiving password
                    std::string nicknamePrompt = "Please enter your nickname:\r\n";
                    send(new_client.getFd() , nicknamePrompt.c_str(), nicknamePrompt.size(), 0);
                    new_client.setPasswordReceived(true);
                } else if (new_client.hasPasswordReceived() && !new_client.hasNicknameReceived() && command == "NICK"&&count ==1) {
                    std::string nick;
                    linestream >> nick;  // Read the nickname
                    new_client.setNickname(nick);
                std::cout<<"my nick: "<<nick<<std::endl;

                    // Prompt for username after receiving nickname
                    std::string usernamePrompt = "Please enter your username:\r\n";
                    send(new_client.getFd() , usernamePrompt.c_str(), usernamePrompt.size(), 0);
                    count=2;
                    new_client.setNicknameReceived(true);
                } else if (new_client.hasPasswordReceived() && new_client.hasNicknameReceived() && !new_client.hasUsernameReceived() && command == "USER"&&count ==2) {
                    std::string user, mode, unused, realname;
                    linestream >> user;
                    new_client.setUsername(user);
                    std::cout<<"my User: "<<user<<std::endl;

                    // new_client setup is complete, you can now proceed with further handling
                    std::string welcomeMessage = ":FreeSpaceChat 001 " + new_client.getNickname()  + " :Welcome to the IRC server\r\n";
                    std::string yourHostMsg = ":FreeSpaceChat 002 " + new_client.getNickname() + " :Your host is localhost\r\n";
                    std::string createdMsg = ":FreeSpaceChat 003 " + new_client.getNickname()+ " :This server was created just now\r\n";
                    std::string myInfoMsg = ":FreeSpaceChat 004 " + new_client.getNickname() + " FreeSpaceChat v1.0 i\r\n";
                    string ident = "You are now identified as <" + new_client.getUsername() + ">\n" + YELLOW + "Have a nice day!\n" + RESET;
                    string toSend = welcomeMessage + yourHostMsg + createdMsg + myInfoMsg + ident;
                    send(new_client.getFd() , toSend.c_str(), toSend.size(), 0);
                    new_client.setUsernameReceived(true);

                }
            }
        }
        if (new_client.hasNicknameReceived() && new_client.hasPasswordReceived() && new_client.hasUsernameReceived())
            break;
    }
}

//for the hexchat
// void Server::parseClientsInput(int fd, const std::string& data) {
//     std::istringstream stream(data);
//     std::string line;
//     while (std::getline(stream, line)) {
//         std::istringstream linestream(line);
//         std::string command;
//         linestream >> command;

//         if (command == "PASS") {
//             std::string pass;
//             linestream >> pass;

//             // Find the clients and set the password
//             for (auto& clients : clientss) {
//                 if (clients.getFd() == fd) {
//                     clients.setPassword(pass);
//                     std::cout<<"our password "<<pass<<std::endl;
//                     break;
//                 }
//             }
//         } else if (command == "NICK") {
//             std::string nick;
//             linestream >> nick;

//             // Find the clients and set the nickname
//             for (auto& clients : clientss) {
//                 if (clients.getFd() == fd) {
//                     clients.setNickname(nick);
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

//             // Find the clients and set the username and realname
//             for (auto& clients : clientss) {
//                 if (clients.getFd() == fd) {
//                     clients.setUsername(user);
//                     std::cout<<"our Username "<<user<<std::endl;
//                    // clients.setRealname(realname);
//                     break;
//                 }
//             }
//         }
//     }
// }