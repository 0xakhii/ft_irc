#include "Channel.hpp"
#include "Server.hpp"

void	createChannel(string av[2], Channel &ch, string username, string nickname, int fd){
	if (av[0][0] != '#'){
		string toSend = string(ERR) + "Invalid channel name\n" + RESET;
		send(fd, toSend.c_str(), toSend.size(), 0);
	}
	else{
		av[0] = &av[0][1];
		if (ch.hasChannel(av[0])){
			if (ch.getInviteOnly(av[0])){
				if (!ch.isInvited(av[0], username)){
					string toSend = string(ERR) + "You are not invited to this channel\n" + RESET;
					send(fd, toSend.c_str(), toSend.size(), 0);
				}
				else{
					ch.addUser(av[0], username, fd);
					string toSend = ":WEBSERV JOIN #" + string(av[0]) + " :" + username + "\n";
					send(fd, toSend.c_str(), toSend.size(), 0);
				}
			}
			else if (ch.getUserLimit(av[0]) != -1 && ch.getUserList(av[0]).size() >= ch.getUserLimit(av[0])){
				string toSend = string(ERR) + "Channel is full\n" + RESET;
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
			else if (ch.isKeyRequired(av[0])){
				if (av[1] != ch.getChannelKey(av[0])){
					string toSend = string(ERR) + "Invalid channel key\n" + RESET;
					send(fd, toSend.c_str(), toSend.size(), 0);
				}
				else{
					ch.addUser(av[0], username, fd);
					string toSend = ":WEBSERV JOIN #" + string(av[0]) + " :" + username + "\n";
					send(fd, toSend.c_str(), toSend.size(), 0);
				}
			}
			else{
				ch.addUser(av[0], username, fd);
				string toSend = ":WEBSERV JOIN #" + string(av[0]) + " :" + username + "\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
		}
		else{
			if (!ch.addChannel(av[0], username, fd)){
				string toSend = string(ERR) + "Channel already exists\n" + RESET;
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
			else{
				string toSend = ":" + nickname + "!WEBSERV@localhost JOIN :" + string(av[0]) + "\r\n" +\
								":WEBSERV 332 " + nickname + " " + string(av[0]) + " :No topic is set\r\n" +\
								": 333 " + nickname + " " + string(av[0]) + " " + nickname + "\r\n" + \
								":WEBSERV 353 " + nickname + " = #" + string(av[0]) + " @:" + nickname + "\r\n" +\
								":WEBSERV 366 " + nickname + " #" + string(av[0]) + " :End of /NAMES list.\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
		}
	}
}

