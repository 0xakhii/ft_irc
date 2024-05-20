#pragma once

#include "Server.hpp"

using namespace std;

class Channel{
	private:
		vector<string> ChannelNames;
	public:
		vector<string> getChannelNames(){
			return ChannelNames;
		}
		void	setChannelNames(string ChannelName){
			ChannelNames.push_back(ChannelName);
		}
};