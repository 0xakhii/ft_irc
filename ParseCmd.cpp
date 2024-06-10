#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "kickcmd.hpp"
#include "Invitecmd.hpp"


void	splitArgs(string av[2], string args){
	size_t spacePos = args.find_first_of(' ');
	while (spacePos != string::npos && args[spacePos + 1] == ' ')
		spacePos = args.find_first_of(' ', spacePos + 1);
	if (spacePos != string::npos){
		av[0] = args.substr(0, spacePos);
		av[1] = args.substr(spacePos + 1);
	}
	else{
		av[0] = args;
		av[1].clear();
	}
	if (av[1][av[1].length() - 1] == '\n')
		av[1].pop_back();
	if (av[0][av[0].length() - 1] == '\n')
		av[0].pop_back();
}
string getUserbyfd(Server& serv, int fd) {
	for (size_t i = 0; i < serv.clients.size(); i++) {
		if (serv.clients[i].getFd() == fd)
			return serv.clients[i].getUsername();
	}
	return "";
}

void	ParseCmd(string cmd, Server& serv, int fd){
	KickCmd k;
	Invitecmd inv;
	std::string cmd_tmp = cmd.substr(cmd.find_first_of(' ') + 1);
	if (cmd.empty())
		return;
	else{
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		cmd = cmd.substr(0, cmd.find_first_of(' '));
		string av[2];
		splitArgs(av, args);
		if (cmd == "JOIN"){ // Join a channel. If the channel specified does not exist, a new one will be created with the given name.
			for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getFd() == fd)
					createChannel(av, serv.ch, serv.clients[i].getUsername(), serv.clients[i].getNickname(), serv.clients[i].getFd());
			}
		}
		else if (cmd == "INVITE"){ // Invite a user to a channel.
			inv.client_name = getUserbyfd(serv, fd);
			inv.invite(av, serv.ch, fd);
		}
		else if (cmd == "KICK"){ // Kick a user from the channel.
			k.client_name = getUserbyfd(serv, fd);
			k.kick(cmd_tmp, serv.ch, fd);
		}
		else if (cmd == "TOPIC"){ // Change or view the topic of the given channel.
			// check the channel where the user run /topic
			if (av[0][0] == '#'){
				if (av[1].empty()){ // print the channel topic
					if (av[0][av[0].length() - 1] == '\n')
						av[0].pop_back();
					string topic = string(YELLOW) + av[0] + ": " + RESET + serv.ch.getTopic(&av[0][1]);
					send(fd, topic.c_str(), topic.size(), 0);
				}
				else{
					for(size_t i = 0; i < serv.clients.size(); i++){
						if (serv.clients[i].getFd() == fd){
							if (!serv.ch.setTopic(&av[0][1], av[1], serv.clients[i].getUsername())){
								string toSend = string(RED) + "You are not allowed to change the topic\n" + RESET;
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
						}
					}
				}
			}
			else{
				string toSend = string(RED) + "Invalid channel name\n" + RESET;
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
		}
		else if (cmd == "MODE"){ // Set or remove options (or modes) to a given target.
			char modeSign = av[1][0];
			char modeFlag = av[1][1];
			if (av[0][0] == '#'){
				av[0] = &av[0][1];
				size_t i = 0;
				av[1] = &av[1][2];
				while(av[1][i] == ' ')
					i++;
				av[1] = &av[1][i];
				if (av[1][av[1].length() - 1] == '\n')
					av[1].pop_back();
				if (modeSign == '+'){
					switch (modeFlag)
					{
						case 'o': // Give channel operator privilege
							for(size_t i = 0; i < serv.clients.size(); ++i){
								// check for spaces before and newline in the end;
								if (serv.clients[i].getUsername() == av[1]){
									if (serv.ch.addOperator(av[0], serv.clients[i].getUsername())){
										string toSend = string(YELLOW) + "You are now an operator\n" + RESET;
										send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
									}
								}
							}
							break;
						case 'i': // Set Invite-only channel
							serv.ch.setInviteOnly(av[0]);
							break;
						case 'l': // Set the user limit to channel
							if (!av[1].empty()) {
								int limit = stoi(av[1]);
								serv.ch.setUserLimit(av[0], limit);
							}
							else {
								string toSend = string(RED) + "User limit not specified\n" + RESET;
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
							break;
						case 'k': // Set the channel key (password)
							if (!av[1].empty()) {
								serv.ch.setChannelKey(av[0], av[1]);
							}
							else{
								string toSend = string(RED) + "Channel key not specified\n" + RESET;
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
							break;
						case 't': // Set the restrictions of the TOPIC command to channel operators
							serv.ch.setTopicRestrictions(av[0]);
							break;
						default:
							string toSend = string(RED) + "Invalid Mode Flag\n" + RESET;
							send(fd, toSend.c_str(), toSend.size(), 0);
							break;
					}
				}
				else if (modeSign == '-'){
					switch (modeFlag)
					{
						case 'o': // take channel operator privilege
							for(size_t i = 0; i < serv.clients.size(); i++){
								if (serv.clients[i].getFd() == fd)
									serv.ch.removeOperator(av[0], serv.clients.back().getUsername());
							}
							break;
						case 'i': // remove Invite-only channel
							serv.ch.removeInviteOnly(av[0]);
							break;
						case 'l': // remove the user limit to channel
							serv.ch.removeUserLimit(av[0]);
							break;
						case 'k': // remove the channel key (password)
							serv.ch.removeChannelKey(av[0]);
							break;
						case 't': // remove the restrictions of the TOPIC command to channel operators
							serv.ch.removeTopicRestrictions(av[0]);
							break;
						default:
							string toSend = string(RED) + "Invalid Mode Flag\n" + RESET;
							send(fd, toSend.c_str(), toSend.size(), 0);
							break;
					}
				}
			}
		}
		else if (cmd == "MSG" || cmd == "PRIVMSG"){ // Send private messages between users.
			if (av[1][0] == ':')
				av[1] = &av[1][1];
			if (av[0][0] == '#'){
				if (serv.ch.hasChannel(&av[0][1])){
					for(size_t i = 0; i < serv.clients.size(); i++){
						if (fd == serv.clients[i].getFd())
							serv.ch.broadcastMessage(&av[0][1], av[1], fd, serv.clients[i].getNickname());
					}
					return;
				}
				else{
					string toSend = string(RED) + "Channel not found\n" + RESET;
					send(fd, toSend.c_str(), toSend.size(), 0);
					return;
				}
			}
			for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getUsername() == av[0]){
					string toSend = ":" + serv.clients[i].getNickname() + "!~" + serv.clients[i].getUsername() + \
									"@localhost PRIVMSG " + av[0] + " :" + av[1] + "\r\n"; 
					send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
					return;
				}
			}
			string toSend = string(RED) + "User not found\n" + RESET;
			send(fd, toSend.c_str(), toSend.size(), 0);
		}
		else if (cmd == "QUIT"){ // Terminate a client’s connection to the server.
			cout << "quit message: " << av[0] << endl;
			serv.ClearClients(fd);
			close(fd);
			cout << "Client <" << fd << "> Disconnected\n";
		}
		else if (cmd == "WHOIS"){
			for (size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getNickname() == av[0]){
					string toSend = av[0] + " 318 :End of /WHOIS list\r\n";
					send(fd, toSend.c_str(), toSend.size(), 0);
				}

			}
		}
		else if (cmd == "PING"){
			string toSend = "PONG irc.server.com :\r\n";
			send(fd, toSend.c_str(), toSend.size(), 0);
		}
	}
}
