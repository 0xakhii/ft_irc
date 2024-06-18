#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "kickcmd.hpp"
#include "Invitecmd.hpp"

//fenetre f privmsg

void take_arguments(std::string args, std::string av[2])
{
	int i = 0;
	int j = 0;

	while(args.size()-2>i && j<2)
	{
		if(args[i] != ' ')
			av[j] += args[i];
		else
			j++;
		i++;

	}
}

string getNickbyfd(Server& serv, int fd) {
	for (size_t i = 0; i < serv.clients.size(); i++) {
		if (serv.clients[i].getFd() == fd)
			return serv.clients[i].getNickname();
	}
	return "";
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
	std::cout<<"cmd *******************: "<<cmd<<std::endl;
	std::string cmd_tmp = cmd.substr(cmd.find_first_of(' ') + 1);
	if (cmd.empty())
		return;
	else{
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		//std::cout<<"++++++++args : "<<args<<"size==>>"<<args.size()<<std::endl;
		//exit(0);
		cmd = cmd.substr(0, cmd.find_first_of(' '));
		string av[2];
		take_arguments(args, av);
		//std::cout<<"av[0] : "<<av[0]<<"av[0].size : "<<av[0].size()<<std::endl;
		//std::cout<<"av[1] : "<<av[1]<<"av[1].size : "<<av[1].size()<<std::endl;
		//splitArgs(av, args);
		if (cmd == "JOIN"){ // Join a channel. If the channel specified does not exist, a new one will be created with the given name.
			for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getFd() == fd)
					createChannel(av, serv.ch, serv.clients[i].getUsername(), serv.clients[i].getFd());
			}
			std::cout<<"channelsize----->>"<<serv.ch.getChannels(av[0].substr(1)).size()<<std::endl;
		}
		else if (cmd == "INVITE"){ // Invite a user to a channel.
			inv.client_name = getUserbyfd(serv, fd);
			inv.invite(av, serv.ch, fd);
		}
		else if (cmd == "KICK"){ // Kick a user from the channel.
			k.client_name = getUserbyfd(serv, fd);
			k.kick(cmd_tmp, serv.ch, fd);
		}
		else if (cmd == "TOPIC")
		{
			if(serv.ch.getTopicRestrictions(av[0]))
			{
			if (serv.ch.isOperator(av[0],getNickbyfd(serv,fd)))
			{
				av[1].erase(0,1);
				serv.ch.setTopic(av[0], av[1], getNickbyfd(serv, fd));
				string topic = ":localhost 332 " + getNickbyfd(serv,fd) + " " + av[0] + " :" + serv.ch.getTopic(av[0]) + "\r\n";
				send(fd, topic.c_str(), topic.size(), 0);
				std::map<std::string, int> vec2  = serv.ch.getChannels(av[0]);
				std::map<std::string, int> ::iterator it5  =vec2.begin();
				for(it5;it5!=vec2.end();it5++)
				{
					if(it5->second != fd)
						send(it5->second, topic.c_str(), topic.size(), 0);
					else
						continue;
				}
			}
			else
			{
				string toSend = ":localhost 482 " + av[0] + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				
			}
			}
			else
			{
				av[1].erase(0,1);
				serv.ch.setTopic(av[0], av[1], getNickbyfd(serv, fd));
				string topic = ":localhost 332 " + getNickbyfd(serv,fd) + " " + av[0] + " :" + serv.ch.getTopic(av[0]) + "\r\n";
				send(fd, topic.c_str(), topic.size(), 0);
				std::map<std::string, int> vec2  = serv.ch.getChannels(av[0]);
				std::map<std::string, int> ::iterator it5  =vec2.begin();
				for(it5;it5!=vec2.end();it5++)
				{
					if(it5->second != fd)
						send(it5->second, topic.c_str(), topic.size(), 0);
					else
						continue;
				}
			}
		}
		else if (cmd == "MODE"){ // Set or remove options (or modes) to a given target.
			char modeSign = av[1][0];
			char modeFlag = av[1][1];
			if (av[0][0] == '#'){
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
										string toSend = "You are now an operator\r\n" ;
										send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
									}
								}
							}
							break;
						case 'i': // Set Invite-only channel
							serv.ch.setInviteOnly(av[0], true, getNickbyfd(serv, fd),fd);
							
							break;
						case 'l': // Set the user limit to channel
							if (!av[1].empty()) {
								int limit = stoi(av[1]);
								serv.ch.setUserLimit(av[0], limit);
							}
							else {
								string toSend ="User limit not specified\r\n";
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
							break;
						case 'k': // Set the channel key (password)
							if (!av[1].empty()) {
								serv.ch.setChannelKey(av[0], av[1]);
							}
							else{
								string toSend = "Channel key not specified\r\n" ;
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
							break;
						case 't': // Set the restrictions of the TOPIC command to channel operators
							serv.ch.setTopicRestrictions(av[0]);
							break;
						default:
							string toSend =  "Invalid Mode Flag\r\n";
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
							serv.ch.removeInviteOnly(av[0], getNickbyfd(serv, fd),fd);
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
							string toSend ="Invalid Mode Flag\r\n";
							send(fd, toSend.c_str(), toSend.size(), 0);
							break;
					}
				}
			}
		}
		else if (cmd == "MSG" || cmd == "PRIVMSG"){ // Send private messages between users.
			
			std::map<string,int> ss = serv.ch.getChannels(av[0]);	
			if(av[0][0] == '#')
			{
				std::cout << "av[0]=============================>>" << av[0] << std::endl;
				msgtochannel(av, fd, serv);
				return;
			}
			else
			{
				//std::cout<<"here"<<std::endl;
				for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getUsername() == av[0]){
					av[1].erase(0,1);
					string toSend = ":" + getNickbyfd(serv, fd) + "!~" + getUserbyfd(serv, fd) + "@localhost PRIVMSG " + av[0] + " :" + av[1] + "\r\n";
					send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
					return;
				}
			}
			string toSend = "User not found\r\n";
			send(fd, toSend.c_str(), toSend.size(), 0);
			}
		}
		else if (cmd == "QUIT"){ // Terminate a client’s connection to the server.
			cout << "quit message: " << av[0] << endl;
			serv.ClearClients(fd);
			close(fd);
			cout << "Client <" << fd << "> Disconnected\n";
		}
		else if (cmd == "PING"){
			string toSend = "PONG irc.server.com :hello\r\n";
			send(fd, toSend.c_str(), toSend.size(), 0);
		}
	}
}

// std::vector<std::string> take_args(std::string args)
// {
// 	std::vector<std::string> arg;
// 	std::string tmp;
// 	for (size_t i = 0; i < args.length(); i++)
// 	{
// 		if (args[i] == ' ')
// 		{
// 			arg.push_back(tmp);
// 			tmp.clear();
// 		}
// 		else
// 			tmp += args[i];
// 	}
// 	arg.push_back(tmp);
// 	return (arg);
// }

void msgtochannel(std::string av[2],int fd,Server &serv)
{
	//std::vector<std::string> arg = take_args(args);
	std::string ch = av[0];
	std::string msg = av[1];
	std::cout << "channel : " << ch << " msg : " << msg << std::endl;
	//ch.erase(0,1);
	std::map<std::string, int> members = serv.ch.getChannels(ch);
	std::cout << "members size : " << members.size() << std::endl;
	msg.erase(0,1);
	//msg = ":" + cli.nickName + " PRIVMSG " + req.arg[0] + " :" + str + "\r\n";
	string res = ":" + getNickbyfd(serv, fd) + " PRIVMSG " + ch + " :" + msg + "\r\n";
	// string res = ":" + getNickbyfd(serv, fd) + "!" + getUserbyfd(serv,fd) + "@" + getNickbyfd(serv, fd) + " PRIVMSG " + ch + " :" + msg + "\r\n";
	for (std::map<std::string, int>::iterator it = members.begin(); it != members.end(); it++)
	{
		std::cout << "nicks : " << it->first <<"size==" << it->first.size() << std::endl;
		if (it->second != fd)
		{
			//msg = ":" + cli.nickName + " PRIVMSG " + req.arg[0] + " :" + str + "\r\n";
			send(it->second, res.c_str(), res.size(), 0);
		}
		else
			continue;
	}
}