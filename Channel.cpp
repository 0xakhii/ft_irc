#include "Channel.hpp"

void	createChannel(string arg, Channel &ch, string username, int fd){
	arg = arg.substr(arg.find_first_not_of(' '), arg.find_first_of('\n'));
	arg = arg.substr(arg.find_first_not_of(' '), arg.find_first_of('\n'));
	if (arg[0] != '#')
		throw runtime_error(string(ERR) + "Invalid Channel Name\n" + RESET);
	else{
		arg = &arg[1];
		cout << "channel name: " << arg << endl;
		if (ch.hasChannel(arg)){
			// check if user can enter this channel
			cout << arg << endl;
			ch.addUser(arg, username, fd);
			cout << "here\n";
		}
		else{
			if (!ch.addChannel(arg, username, fd)){
				cout << ERR << "Channel already exists\n" << RESET;
				return ;
			}
		}
	}
}

