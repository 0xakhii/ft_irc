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
	if (cmd.empty())
		throw runtime_error(string(ERR) + "Invalid command\n" + RESET);
	else if (cmd[0] == '/'){
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		cmd = cmd.substr(1, cmd.find_first_of(' ') - 1);
		string av[2];
		splitArgs(av, args);
		cout << "cmd: " << cmd << endl;
		if (cmd == "JOIN"){ // Join a channel. If the channel specified does not exist, a new one will be created with the given name.
			for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getFd() == fd)
					createChannel(av[0], serv.ch, serv.clients[i].getUsername(), serv.clients[i].getFd());
			}
		}
		else if (cmd == "INVITE"){ // Invite a user to a channel.
			inv.client_name = getUserbyfd(serv, fd);
			inv.invite(av, serv.ch, fd);
		}
		else if (cmd == "KICK"){ // Kick a user from the channel.
			k.client_name = getUserbyfd(serv, fd);
			k.kick(av, serv.ch, fd);
		}
		else if (cmd == "TOPIC"){ // Change or view the topic of the given channel.
			// check the channel where the user run /topic
			if (av[0][0] == '#'){
				if (av[1].empty()){ // print the channel topic
					string topic = string(YELLOW) + "Channel: " + av[0] + "TOPIC: " + RESET + serv.ch.getTopic(&av[0][1]);
					send(fd, topic.c_str(), topic.size(), 0);
				}
				else{
					for(size_t i = 0; i < serv.clients.size(); i++){
						if (serv.clients[i].getFd() == fd){
							if (!serv.ch.setTopic(&av[0][1], av[1], serv.clients[i].getUsername()))
								cout << ERR <<  "Can't set a new topic\n" << RESET;
						}
					}
				}
			}
			else
				cout << ERR <<  "Invalid Channel Name\n" << RESET;
		}
		else if (cmd == "MODE"){ // Set or remove options (or modes) to a given target.
			char modeSign = av[1][0];
			char modeFlag = av[1][1];
			if (av[0][0] == '#'){
				av[0] = &av[0][1];
				av[1] = &av[1][2];
				if (modeSign == '+'){
					switch (modeFlag)
					{
						case 'o': // Give channel operator privilege
							for(size_t i = 0; i < serv.clients.size(); i++){
								// check for spaces before and newline in the end;
								cout << "username: " << av[1] << endl;
								if (serv.clients[i].getUsername() == av[1]){
									if (serv.ch.addOperator(av[0], serv.clients[i].getUsername())){
										string toSend = string(YELLOW) + "You are now an operator\n" + RESET;
										send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
									}
								}
							}
							break;
						 // Set Invite-only channel
						case 'i': // Set Invite-only channel
							serv.ch.setInviteOnly(av[0]);
							break;
						case 'l': // Set the user limit to channel
							if (!av[1].empty()) {
								int limit = stoi(av[1]);
								serv.ch.setUserLimit(av[0], limit);
							} else {
								cout << ERR << "User limit not specified\n" << RESET;
							}
							break;
						case 'k': // Set the channel key (password)
							if (!av[1].empty()) {
								serv.ch.setChannelKey(av[0], av[1]);
							} else {
								cout << ERR << "Channel key not specified\n" << RESET;
							}
							break;
						case 't': // Set the restrictions of the TOPIC command to channel operators
							serv.ch.setTopicRestrictions(av[0]);
							break;
						default:
							throw runtime_error(string(ERR) + "Invalid Mode Flag\n" + RESET);
							cout << ERR << "Invalid Mode Flag\n" << RESET;
							break;
					}
					cout << "mode <+>: " << modeFlag << endl;
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
							throw runtime_error(string(ERR) + "Invalid Mode Flag\n" + RESET);
							break;
					}
					cout << "mode <->: " << modeFlag << endl;
				}
			}
		}
		else if (cmd == "MSG"){ // Send private messages between users.
			cout << "Nickname: " << av[0] << endl;
			cout << "message: " << av[1] << endl;
			for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getUsername() == av[0]){
					string toSend = string(YELLOW) + "PRIVATE MESSAGE FROM <" + getUserbyfd(serv, fd) + "> " + RESET + av[1]; 
					send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
				}
			}
		}
		else if (cmd == "QUIT"){ // Terminate a client’s connection to the server.
			cout << "quit message: " << av[0] << endl;
			serv.ClearClients(fd);
			close(fd);
			cout << "Client <" << fd << "> Disconnected\n";
			
		}
		else
			cout << "why?\n";
	}
	else if (!cmd.empty()){
		for(size_t i = 0; i < serv.clients.size(); i++){
			if (serv.clients[i].getFd() == fd)
				serv.ch.broadcastMessage(serv.ch.getJoinedChannel(serv.clients[i].getUsername()), cmd, fd, serv.clients[i].getUsername());
		}
	}
}
