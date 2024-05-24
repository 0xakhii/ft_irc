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

void	ParseCmd(string cmd, Channel &ch, Server serv, int fd){
	(void)serv;
	string username; // temp string until i get the clients data
	if (cmd.empty())
		throw runtime_error(string(ERR) + "Invalid command\n" + RESET);
	else{
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		cmd = cmd.substr(1, cmd.find_first_of(' ') - 1);
		string av[2];
		splitArgs(av, args);
		cout << "cmd: " << cmd << endl;
		if (cmd == "JOIN"){ // Join a channel. If the channel specified does not exist, a new one will be created with the given name.
			createChannel(av[0], ch);
		}
		else if (cmd == "INVITE"){ // Invite a user to a channel.
		}
		else if (cmd == "KICK"){ // Kick a user from the channel.
		}
		else if (cmd == "TOPIC"){ // Change or view the topic of the given channel.
			// check the channel where the user run /topic
			if (av[0][0] == '#'){
				if (av[1].empty())// print the channel topic
					cout << av[0] << ": " << ch.getTopic(&av[0][1]) << endl;
				else{
					if (!ch.setTopic(av[0], av[1], username))
						throw runtime_error(string(ERR) + "Can't set a new topic\n" + RESET);
				}
			}
			else
				throw runtime_error(string(ERR) + "Invalid Channel Name\n" + RESET);
		}
		else if (cmd == "MODE"){ // Set or remove options (or modes) to a given target.
			char modeSign = av[1][0];
			char modeFlag = av[1][1];
			if (av[0][0] == '#'){
				cout << "for channels: " << av[0] << endl;
				if (modeSign == '+'){
					if (modeFlag == 'o')
						ch.addOperator(av[0], username);
					cout << "mode <+>: " << modeFlag << endl;
				}
				else if (modeSign == '-'){
					if (modeFlag == 'o')
						ch.removeOperator(av[0], username);
					cout << "mode <->: " << modeFlag << endl;
				}
			}
			else{
				cout << "for users: " << av[0] << endl;
				if (modeSign == '+'){
					cout << "mode <+>: " << modeFlag << endl;
				}
				else if (modeSign == '-'){
					cout << "mode <->: " << modeFlag << endl;
				}
			}
		}
		else if (cmd == "MSG"){ // Send private messages between users.
			cout << "Nickname: " << av[0] << endl;
			cout << "message: " << av[1] << endl;
		}
		else if (cmd == "QUIT"){ // Terminate a client’s connection to the server.
			cout << "quit message: " << av[0] << endl;
			serv.ClearClients(fd);
			close(fd);
			cout << "Client: " << fd << "Disconnected\n";
			
		}
		else
			cout << "why?\n";
	}
}
