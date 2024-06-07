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
#include <cstdlib>

void Server::parseClientInput(int fd, const std::string& data) {
    std::istringstream stream(data);  // Create an input string stream from the data string
    std::string line;
    while (std::getline(stream, line)) {  // Read each line from the stream
        std::istringstream linestream(line);  // Create an input string stream from the line
        std::string command;
        linestream >> command;  // Read the command from the line
    if(command=="quit")
    {
        std::cout  <<RED<< "Client <" << fd << "> Disconnected" << std::endl;
		close(fd);
    }
        for (size_t i = 0; i < clients.size(); ++i) {
             Client& client = clients[i];
            if (client.getFd() == fd) {
                if (command == "CAP")
                {
                    std::string passe_send = "Please enter your password:\r\n";
                    send(fd, passe_send.c_str(), passe_send.size(), 0);
                }
                if (!client.hasPasswordReceived() && command == "PASS" && client.count ==0) {
                    std::string passe;
                    linestream >> passe;  // Read the password
                    if(passe!=pass ||  passe.empty())
                    {
                        std::string pass_err;
                        if(passe.empty())
                        {
                            std::cout<<"hello \n";
                             pass_err=ERR_NEEDMOREPARAMS(line);
                        }
                        else
                             pass_err=ERR_PASSWDMISMATCH();
                        std::string passe_send = "Please enter your password \r\n";
                        send(fd,pass_err.c_str(),pass_err.size(),0);
                        send(fd, passe_send.c_str(), passe_send.size(), 0);
                        continue;
                    }
                    client.setPassword(passe);
                    client.setPasswordReceived(true);
                    client.count = 1;
                    std::string nicknamePrompt = "please enter the nickname:\r\n";
                    send(fd, nicknamePrompt.c_str(), nicknamePrompt.size(), 0);
                } else if (client.hasPasswordReceived() && !client.hasNicknameReceived() && command == "NICK"&&client.count ==1) {
                    std::string nick;
                    linestream >> nick;  // Read the nickname
                    if(!prsNickname(nick,fd))
                        continue;
                    client.setNickname(nick);
                    client.setNicknameReceived(true);
                  std::string usernamePrompt = "Please enter your username:\r\n";
                    send(fd, usernamePrompt.c_str(), usernamePrompt.size(), 0);
                    client.count = 2;
                } else if (client.hasPasswordReceived() && client.hasNicknameReceived() && !client.hasUsernameReceived() && command == "USER"&&client.count ==2) {
                    std::istringstream iss(line);
                    std::string comd, username, realname;
                    std::string unusedInt;
                    std::string unusedChar;
                    int i = 0;
                for (std::string each; std::getline(iss, each, ' ');)
                {
                    if(i == 1)
                        username = each;
                    else if(i == 2)
                        unusedInt= each;
                    else if(i == 3)
                        unusedChar = each;
                    else if(i == 4)
                        realname = each;
                    i++;
                }
                if(!parsUSer(i,unusedInt,unusedChar,command,fd,realname,username)){
                    std::string usernamePrompt = "Please enter your username:\r\n";
                    send(fd, usernamePrompt.c_str(), usernamePrompt.size(), 0);
                    continue;
                }
                client.setUsername(username);
                client.setRealname(realname);
                client.setUsernameReceived(true);
                send_welcome_message(fd,client);
                }
                }
                break;
            }
        }}

void Server::send_welcome_message(int fd,Client client)
{
    // Welcome message with color code
    std::string welcomeMessage = ":WEBSERV 001 " + client.getNickname() + " :Welcome to the WEBSERV Network, " + client.getNickname() + " [!" + client.getNickname() + "@localhost]\r\n";
    std::string yourHostMsg = ":WEBSERV 002 " + client.getNickname() + " :Your host is WEBSERV\r\n";
    std::string createdMsg = ":WEBSERV 003 " + client.getNickname() + " :This server was created just now\r\n";
    std::string myInfoMsg = ":WEBSERV 004 " + client.getNickname() + " WEBSERV v1.0 i\r\n";

    // Send each message
    send(fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
    send(fd, yourHostMsg.c_str(), yourHostMsg.size(), 0);
    send(fd, createdMsg.c_str(), createdMsg.size(), 0);
    send(fd, myInfoMsg.c_str(), myInfoMsg.size(), 0);


}