#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "kickcmd.hpp"
#include "Invitecmd.hpp"

void take_arguments(string args, string av[10000])
{
	int i = 0;
	int j = 0;

	while(args.size()-2>i)
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
	string cmd_tmp = cmd.substr(cmd.find_first_of(' ') + 1);
	if (cmd.empty())
		return;
	else{
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		string lastArg = args.substr(args.find_last_of(' ') + 1);
		lastArg = lastArg.substr(0, lastArg.size() - 2);
		cmd = cmd.substr(0, cmd.find_first_of(' '));
		string av[10000];
		take_arguments(args, av);
		if (cmd == "JOIN"){ // Join a channel. If the channel specified does not exist, a new one will be created with the given name.
			createChannel(av, serv.ch, getNickbyfd(serv, fd), fd);
		}
		else if (cmd == "INVITE"){ // Invite a user to a channel.
			inv.client_name = getUserbyfd(serv, fd);
			inv.invite(av, serv.ch, fd);
		}
		else if (cmd == "KICK"){ // Kick a user from the channel.
			k.client_name = getUserbyfd(serv, fd);
			k.kick(cmd_tmp, serv.ch, fd);
		}
		else if (cmd == "TOPIC"){
			if (!av[1].empty()){
				if(serv.ch.getTopicRestrictions(av[0])){
					if (serv.ch.isOperator(av[0],getNickbyfd(serv,fd))){
						av[1].erase(0,1);
						serv.ch.setTopic(av[0], av[1], getNickbyfd(serv, fd));
						string topic = ":localhost 332 " + getNickbyfd(serv,fd) + " " + av[0] + " :" + serv.ch.getTopic(av[0]) + "\r\n";
						send(fd, topic.c_str(), topic.size(), 0);
						map<string, int> vec2  = serv.ch.getChannels(av[0]);
						map<string, int> ::iterator it5  =vec2.begin();
						for(it5;it5!=vec2.end();it5++){
							if(it5->second != fd)
								send(it5->second, topic.c_str(), topic.size(), 0);
							else
								continue;
						}
					}
					else{
						string toSend = ":localhost 482 " + av[0] + " :You're not channel operator\r\n";
						send(fd, toSend.c_str(), toSend.size(), 0);
						
					}
				}
				else{
					av[1].erase(0,1);
					serv.ch.setTopic(av[0], av[1], getNickbyfd(serv, fd));
					string topic = ":localhost 332 " + getNickbyfd(serv,fd) + " " + av[0] + " :" + serv.ch.getTopic(av[0]) + "\r\n";
					send(fd, topic.c_str(), topic.size(), 0);
					map<string, int> vec2  = serv.ch.getChannels(av[0]);
					map<string, int> ::iterator it5  =vec2.begin();
					for(it5;it5!=vec2.end();it5++){
						if(it5->second != fd)
							send(it5->second, topic.c_str(), topic.size(), 0);
						else
							continue;
					}
				}
			}
			else{
				string toSend = ":localhost 331 " + getNickbyfd(serv,fd) + " " + av[0] + " :" + serv.ch.getTopic(av[0]) + "\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
			}
		}
		else if (cmd == "MODE"){ // Set or remove options (or modes) to a given target.
			char modeSign = av[1][0];
			char modeFlag = av[1][1];
			if (av[0][0] == '#'){
				if (modeSign == '+'){
					switch (modeFlag){
						case 'o': // Give channel operator privilege
							for(size_t i = 0; i < serv.clients.size(); ++i){
								if (serv.clients[i].getNickname() == lastArg){
									if (serv.ch.addOperator(av[0], serv.clients[i].getNickname(), fd)){
										string toSend = ":localhost 351 " + serv.clients[i].getNickname() + "!" \
										+ serv.clients[i].getUsername() + "@localhost MODE #" + av[0] + " +o " + \
										serv.clients[i].getNickname() + "\nYou are now an operator\r\n" ;
										send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
									}
								}
							}
							break;
						case 'i': // Set Invite-only channel
							serv.ch.setInviteOnly(av[0], true, getNickbyfd(serv, fd),fd);
							
							break;
						case 'l': // Set the user limit to channel
							if (!av[2].empty()) {
								std::cout << "lastArg: ==============" << lastArg << std::endl;
								//hna rh tad5l wa5a matansetech largument l5er otatexisti hit stoi tatreturni chi haja machi talhih
								int limit = stoi(av[2]);
								serv.ch.setUserLimit(av[0], limit, getNickbyfd(serv, fd),fd);
							}
							else {
								//": 461 " + cmd + " :Not enough parameters\r\n"
								string toSend = ": 461 " + cmd + " :Not enough parameters\r\n";
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
							break;
						case 'k':
						
							if (!av[2].empty()) {
								std::cout << "lastArg: ==============" << lastArg << std::endl; // Set the channel key (password)
								serv.ch.setChannelKey(av[0], lastArg, getNickbyfd(serv, fd),fd);
							}
							else{
								std::cout<<"cmdsize: ================="<<cmd.size()<<std::endl;
								string toSend = ": 461 " + cmd + " :Not enough parameters\r\n";
								send(fd, toSend.c_str(), toSend.size(), 0);
							}
							break;
						case 't': // Set the restrictions of the TOPIC command to channel operators
							serv.ch.setTopicRestrictions(av[0], getNickbyfd(serv, fd),fd);
							break;
						default:
							string toSend = ":localhost 472 " + getNickbyfd(serv,fd) +  " :Invalid Mode Flag\r\n";
							send(fd, toSend.c_str(), toSend.size(), 0);
							break;
					}
				}
				else if (modeSign == '-'){
					switch (modeFlag){
						case 'o': // take channel operator privilege
							for(size_t i = 0; i < serv.clients.size(); i++){
								if (serv.clients[i].getNickname() == lastArg)
									serv.ch.removeOperator(av[0], serv.clients[i].getNickname(), fd);
							}
							break;
						case 'i': // remove Invite-only channel
							serv.ch.removeInviteOnly(av[0], getNickbyfd(serv, fd),fd);
							break;
						case 'l': // remove the user limit to channel
							serv.ch.removeUserLimit(av[0], getNickbyfd(serv, fd), fd);
							break;
						case 'k': // remove the channel key (password)
							serv.ch.removeChannelKey(av[0], getNickbyfd(serv, fd), fd);
							break;
						case 't': // remove the restrictions of the TOPIC command to channel operators
							serv.ch.removeTopicRestrictions(av[0], getNickbyfd(serv, fd), fd);
							break;
						default:
							string toSend = ":localhost 472 " + getNickbyfd(serv,fd) + " :Invalid Mode Flag\r\n";
							send(fd, toSend.c_str(), toSend.size(), 0);
							break;
					}
				}
			}
		}
		else if (cmd == "MSG" || cmd == "PRIVMSG"){ // Send private messages between users.
			map<string,int> ss = serv.ch.getChannels(av[0]);	
			if(av[0][0] == '#'){
				msgtochannel(av, fd, serv);
				return;
			}
			else{
				string m = relaymsg(av);
				for(size_t i = 0; i < serv.clients.size(); i++){
				if (serv.clients[i].getUsername() == av[0]){
					m.erase(0,1);
					string toSend = ":" + getNickbyfd(serv, fd) + "!~" + getUserbyfd(serv, fd) + "@localhost PRIVMSG " + av[0] + " :" + m + "\r\n";
					send(serv.clients[i].getFd(), toSend.c_str(), toSend.size(), 0);
					return;
				}
			}
			string toSend = av[0] + " 401 :No suck Nickname/Channel\r\n";
			send(fd, toSend.c_str(), toSend.size(), 0);
			}
		}
		else if (cmd == "QUIT"){ // Terminate a client’s connection to the server.
			serv.ClearClients(fd);
			close(fd);
			cout << "Client <" << fd << "> Disconnected\n";
		}
		else if (cmd == "PING"){
			string toSend = ":localhost " + getNickbyfd(serv, fd) + "!"\
			+ getUserbyfd(serv, fd) + " PONG " + av[0] + "\r\n";
			send(fd, toSend.c_str(), toSend.size(), 0);
		}
	}
}

std::string relaymsg(string av[10000])
{
	string msg;
	for (int i = 1; av[i] != ""; i++)
	{
		msg += av[i] + " ";
	}
	return (msg);
}

void msgtochannel(string av[10000],int fd,Server &serv){
	string ch = av[0];
	string msg = relaymsg(av);
	std::cout << "msg--------------: " << msg << std::endl;
	map<string, int> members = serv.ch.getChannels(ch);
	msg.erase(0,1);
	string res = ":" + getNickbyfd(serv, fd) + " PRIVMSG " + ch + " :" + msg + "\r\n";
	for (map<string, int>::iterator it = members.begin(); it != members.end(); it++)
	{
		if (it->second != fd)
			send(it->second, res.c_str(), res.size(), 0);
		else
			continue;
	}
}