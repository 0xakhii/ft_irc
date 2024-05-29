#include "Server.hpp"
#include "Channel.hpp"

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

string	getUserbyFd(Server serv, int fd){
	for(size_t i = 0; i < serv.clients.size(); i++){
		if (serv.clients[i].getFd() == fd)
			return serv.clients[i].getUser();
	}
	return "";
}

void	ParseCmd(string cmd, Channel &ch, Server serv, int fd){
	(void)serv;
	KickCmd k;
	if (cmd.empty())
		cout << string(ERR) + "Invalid command\n" + RESET;
	else if (cmd[0] == '/'){
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		cmd = cmd.substr(1, cmd.find_first_of(' ') - 1);
		string av[2];
		splitArgs(av, args);
		cout << "cmd: " << cmd << endl;
		if (cmd == "JOIN"){ // Join a channel. If the channel specified does not exist, a new one will be created with the given name.
			for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getFd() == fd)
					createChannel(av[0], ch, serv.clients[i].getUser(), serv.clients[i].getFd());
			}
		}
		else if (cmd == "INVITE"){ // Invite a user to a channel.
		}
		else if (cmd == "KICK"){ // Kick a user from the channel.
			k.kick(av, ch);
		}
		else if (cmd == "TOPIC"){ // Change or view the topic of the given channel.
			// check the channel where the user run /topic
			if (av[0][0] == '#'){
				if (av[1].empty()){ // print the channel topic
					string topic = string(YELLOW) + "Channel: " + av[0] + "TOPIC: " + RESET + ch.getTopic(&av[0][1]);
					send(fd, topic.c_str(), topic.size(), 0);
				}
				else{
					for(size_t i = 0; i < serv.clients.size(); i++){
						if (serv.clients[i].getFd() == fd){
							if (!ch.setTopic(&av[0][1], av[1], serv.clients[i].getUser()))
								cout << string(ERR) + "Can't set a new topic\n" + RESET;
						}
					}
				}
			}
			else
				cout << string(ERR) + "Invalid Channel Name\n" + RESET;
		}
		else if (cmd == "MODE"){ // Set or remove options (or modes) to a given target.
			char modeSign = av[1][0];
			char modeFlag = av[1][1];
			if (av[0][0] == '#'){
				av[0] = &av[0][1];
				if (modeSign == '+'){
					switch (modeFlag)
					{
						case 'o': // Give channel operator privilege
							for(size_t i = 0; i < serv.clients.size(); i++){
								if (serv.clients[i].getFd() == fd){
									ch.addOperator(av[0], serv.clients.back().getUser());
								}
							}
							break;
						case 'i': // Set Invite-only channel
							break;
						case 'l': // Set the user limit to channel
							break;
						case 'k': // Set the channel key (password)
							break;
						case 't': //Set the restrictions of the TOPIC command to channel operators
							break;
						default:
							cout << string(ERR) + "Invalid Mode Flag\n" + RESET;
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
									ch.removeOperator(av[0], serv.clients.back().getUser());
							}
							break;
						case 'i': // remove Invite-only channel
							break;
						case 'l': // remove the user limit to channel
							break;
						case 'k': // remove the channel key (password)
							break;
						case 't': // remove the restrictions of the TOPIC command to channel operators
							break;
						default:
							cout << string(ERR) + "Invalid Mode Flag\n" + RESET;
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
				if (serv.clients[i].getUser() == av[0]){
					string toSend = string(YELLOW) + "PRIVATE MESSAGE FROM " + getUserbyFd(serv, fd) + ": " + RESET + av[1];
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
				ch.broadcastMessage(ch.getJoinedChannel(serv.clients[i].getUser()), cmd, fd, serv.clients[i].getUser());
		}
	}
}
