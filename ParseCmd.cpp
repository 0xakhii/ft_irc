#include "Server.hpp"

void	ParseCmd(string cmd){
	if (cmd[0] != '/' || cmd.empty())
		cerr << "Invalid command\n";
	else{
		string args = cmd.substr(cmd.find_first_of(' ') + 1);
		cmd = cmd.substr(1, cmd.find_first_of(' '));
		string av[2];
		av[0] = args.substr(0, args.find_first_of(' '));
		av[1] = args.substr(args.find_first_of(' ') + 1, args.size());
		cout << "cmd: " << cmd << endl;
		cout << "first arg: " << av[0] << endl;
		cout << "second arg " << av[1] << endl;
		if (cmd == "JOIN"){
			// Join a channel. If the channel specified does not exist, a new one will be created with the given name.
		}
		else if (cmd == "INVITE"){
			// Invite a user to a channel.
		}
		else if (cmd == "KICK"){
			// Kick a user from the channel.
		}
		else if (cmd == "TOPIC"){
			// Change or view the topic of the given channel.
		}
		else if (cmd == "MODE"){
			// Set or remove options (or modes) to a given target.
		}
		else if (cmd == "MSG"){
			// Send private messages between users.
		}
		else if (cmd == "QUIT"){
			// Terminate a client’s connection to the server.
		}
	}
}
