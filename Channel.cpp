#include "Channel.hpp"
#include "Server.hpp"

void	createChannel(string av[2], Channel &ch, string nickname, int fd){
	if (av[0][0] != '#'){
		string toSend = ": 403 " + av[0] + " :Invalid channel\r\n";
		send(fd, toSend.c_str(), toSend.size(), 0);
	}
	else{
		if (ch.hasChannel(av[0])){
			if (ch.getInviteOnly(av[0])){
				if (!ch.isInvited(av[0], nickname)){
					string toSend = ": 473 " + nickname + " " + av[0] + " :Cannot join channel (+i)\r\n";
					send(fd, toSend.c_str(), toSend.size(), 0);
				}
				else
					ch.addUser(av[0], nickname, fd);
			}
			else if (ch.getUserLimit(av[0]) != -1 || ch.getUserList(av[0]).size() >= (size_t)ch.getUserLimit(av[0])){
				string toSend = ": 471 " + nickname + " " + av[0] + " :Channel is full\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
			else if (ch.isKeyRequired(av[0])){
				if (av[1] != ch.getChannelKey(av[0])){
					string toSend = ": 475 " + nickname + " " + av[0] +" :Cannot join channel (+k)\r\n";
					send(fd, toSend.c_str(), toSend.size(), 0);
				}
				else
					ch.addUser(av[0], nickname, fd);
			}
			else
				ch.addUser(av[0], nickname, fd);
		}
		else{
			if (!ch.addChannel(av[0], nickname, fd)){

				string toSend = "Channel already exists\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
			else{
				string toSend = ":" + nickname + "!" + nickname +"@localhost JOIN :" + av[0] + "\r\n";
				string toSend2 = ":localhost 353 " + nickname + " = " + av[0] + " :@" + nickname + "\r\n";
				string toSend3 = ":localhost 366 " + nickname + " " + av[0] + " :End of /NAMES list.\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				send(fd, toSend2.c_str(), toSend2.size(), 0);
				send(fd, toSend3.c_str(), toSend3.size(), 0);
			}
		}
	}
}

