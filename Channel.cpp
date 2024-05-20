#include "Channel.hpp"

void	splitChannelName(string arg, Channel &ch){
	arg = arg.substr(arg.find_first_not_of(' '));
	if (arg[0] != '#')
		cerr << ERR << "Invalid Channel Name\n" << RESET;
	else{
		arg = &arg[1];
		cout << "channel name: " << arg << endl;
		if (find(ch.getChannelNames().begin(), ch.getChannelNames().end(), arg) != ch.getChannelNames().end()){
			// check if user can enter this channel
		}
		else{
			ch.setChannelNames(arg);
		}
	}
}