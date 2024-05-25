#include "Channel.hpp"

void	createChannel(string arg, Channel &ch, string username, int fd){
	arg = arg.substr(arg.find_first_not_of(' '));
	if (arg[0] != '#')
		throw runtime_error(string(ERR) + "Invalid Channel Name\n" + RESET);
	else{
		arg = &arg[1];
		cout << "channel name: " << arg << endl;
		if (find(ch.getChannelNames().begin(), ch.getChannelNames().end(), arg) != ch.getChannelNames().end()){
			// check if user can enter this channel
			ch.addUser(arg, username, fd);
			cout << "here\n";
		}
		else{
			if (!ch.addChannel(arg, username, fd))
				throw runtime_error(string(ERR) + "Channel already exists\n" + RESET);
		}
	}
}