#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <set>
#include"kickcmd.hpp"

//#define ERR "\033[1;31mError:\033[0;0m\n\033[1m"
// #define YELLOW "\033[1;33m"
//#define RESET "\033[0;0m"

using namespace std;

class KickCmd;

class Channel {
	private:
		struct ChannelData {
			string name;
			string admin;
			string topic;
			set<string> operators;
			map<string, int> userList;
			int userLimit;
			vector<string> invitelist;
			bool InviteOnly;
			bool TopicRestrictions;
			string ChannelKey;
			bool isKeyRequired;
		};
		map<string, ChannelData> Channels;
		
	public:
		bool channelexist = false;
		std::map<std::string,int> allclient;
		std::map<std::string,int> getChannels(std::string channel_name){
			std::map<std::string,int> res;
			for (std::map<std::string, ChannelData>::iterator it = Channels.begin(); it != Channels.end(); it++)
			{
				if (it->first.find(channel_name) != std::string::npos)
				{
					channelexist = true;
					res = it->second.userList;
				}
			}
			return res;
		}
		
		bool hasChannel(const string& channelName) const {
			return Channels.count(channelName) > 0;
		}
		bool addChannel(string& channelName, string username, int fd) {

			//channelName
			std::cout<<"|||||"<<channelName<<"|||"<<channelName.size()<<"!!!!!"<<std::endl;
			std::pair<map<string, ChannelData>::iterator, bool> result = Channels.insert({channelName, ChannelData{}});
			if (!result.second) {
				return false;
			}
			result.first->second.name = channelName;
			result.first->second.admin = username;
			result.first->second.topic = "No topic set\n";
			result.first->second.operators.insert(username);
			result.first->second.userList[username] = fd;
			result.first->second.userLimit = -1;
			return true;
		}

		const ChannelData& getChannel(const string& channelName) const {
			if (!hasChannel(channelName)){
				static ChannelData empty;
				return empty;
			}
			return Channels.at(channelName);
		}

		bool setTopic(const string& channelName, const string& newTopic, const string& username) {
			if (!isOperator(channelName, username) && Channels[channelName].TopicRestrictions)
				return false;
			else
				Channels[channelName].topic = newTopic;
				// Channels[channelName].TopicRestrictions = false;
			return true;
		}
		bool getTopicRestrictions(const string& channelName) const {
			return Channels.at(channelName).TopicRestrictions;
		}
		string getTopic(const string& channelName) const {
			if (!hasChannel(channelName))
				return "";
			return Channels.at(channelName).topic;
		}
		void send_responsed(int fd, std::string response){
    	if(send(fd, response.c_str(), response.size(), 0) < 0)
        std::cout<<"SEND FAILED!"<<std::endl;
		}

		bool addUser(const string& channelName, const string& username, int fd) {

			Channels[channelName].userList[username] = fd;
			std::map<string, int>::iterator it;
			std::string member_str = ":localhost 353 " + username + " = " + channelName + " :@" + Channels[channelName].admin + " ";
    		for (it = Channels[channelName].userList.begin(); it != Channels[channelName].userList.end(); ++it)
    		{
				if (it->first == Channels[channelName].admin)
					continue;
				else
    		    	member_str += it->first + " ";
    		}
    		member_str += "\r\n";
			send(fd, member_str.c_str(), member_str.size(), 0);
			// send(fd,   ":" + t.nickName + "!" + t.serverName + "@localhost JOIN :"+ channel + "\r\n"))
			send_responsed(fd, ":" + username + "!" + username + "@localhost JOIN :" + channelName + "\r\n");
			send_responsed(fd, ":localhost 366 " + username + " " + channelName + " :End of /NAMES list.\r\n");
			// broadcastMessage(channelName, "has joind the channel", fd, username);
			return true;
		}
		bool addinviteeduser(const string& chan_name, const string& username) {
			Channels[chan_name].invitelist.push_back(username);
			return true;
		}
		bool removeUser(const string& channelName, const string& username) {
			ChannelData& channelData = Channels[channelName];
			channelData.userList.erase(username);
			return true;
		}
		bool isOperator(const string& channelName, const string& nickname) const {
			return Channels.at(channelName).operators.count(nickname) > 0;
		}
		bool addOperator(const string& channelName, const string& username) {
			Channels[channelName].operators.insert(username);
			return true;
		}
		bool removeOperator(const string& channelName, const string& username) {
			return Channels.at(channelName).operators.erase(username) > 0;
		}
		map<string, int> getUserList(const string& channelName) const {
			if (!hasChannel(channelName)) {
				return map<string, int>();
			}
			return Channels.at(channelName).userList;
		}
		vector<string> getChannelNames() const {
			vector<string> channelNames;
			for (const pair<string, ChannelData>& channel : Channels)
				channelNames.push_back(channel.first);
			return channelNames;
		}
		string getJoinedChannel(const string& username) const {
			string lastJoinedChannel = "";
			for (const pair<string, ChannelData>& channel : Channels) {
				const ChannelData& channelData = channel.second;
				if (channelData.userList.count(username) > 0) {
					lastJoinedChannel = channelData.name;
				}
			}
			return lastJoinedChannel;
		}
		void broadcastMessage(const string& channelName, const string& message, int fd, string username) {
			if (!hasChannel(channelName)) {
				string toSend = "Channel does not exist\r\n" ;
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			const map<string, int>& userList = Channels.at(channelName).userList;
			for (map<string, int>::const_iterator it = userList.begin(); it != userList.end(); ++it) {
				cout << "Broadcasting message to user: " << it->first << endl;
				//":" + nick + "!" + username + "@localhost JOIN :" + channel + "\r\n"
				string toSend = ":" +  username+ "!" + username + "@localhost JOIN :"  + channelName + "\r\n";
				if (fd != it->second){
					send(it->second, toSend.c_str(), toSend.size(), 0);
				}
			}
		}
		void removeInviteOnly(const string& channelName,std::string nickname,int fd) {
			Channels[channelName].InviteOnly = false;
			std::string str = ":" + nickname+ " MODE " + channelName + " -i\r\n";
			send(fd, str.c_str(), str.size(), 0);
		}
		bool getInviteOnly(const string& channelName) const {
			return Channels.at(channelName).InviteOnly;
		}
		bool isInvited(const string& channelName, const string& username) const {
			return find(Channels.at(channelName).invitelist.begin(), Channels.at(channelName).invitelist.end(), username) != Channels.at(channelName).invitelist.end();
		}
		void removeUserLimit(const string& channelName) {
			Channels[channelName].userLimit = -1;
		}
		void removeChannelKey(const string& channelName) {
			Channels[channelName].invitelist.clear();
			Channels[channelName].isKeyRequired = false;
		}
		bool isKeyRequired(const string& channelName) const {
			return Channels.at(channelName).isKeyRequired;
		}
		string getChannelKey(const string& channelName) const {
			return Channels.at(channelName).ChannelKey;
		}
		void removeTopicRestrictions(const string& channelName) {
			Channels[channelName].TopicRestrictions = false;
		}
		void setInviteOnly(const string& channelName,bool inviteOnly,std::string nickname,int fd) {
			Channels[channelName].InviteOnly = inviteOnly;
			std::string str = ":" + nickname+ " MODE " + channelName + " +i\r\n";
			send(fd, str.c_str(), str.size(), 0);
		}
		void setUserLimit(const string& channelName, int limit) {
			Channels[channelName].userLimit = limit;
		}
		int getUserLimit(const string& channelName) const {
			return Channels.at(channelName).userLimit;
		}
		void setChannelKey(const string& channelName, const string& key) {
			Channels[channelName].ChannelKey = key;
			Channels[channelName].isKeyRequired = true;
		}
		void setTopicRestrictions(const string& channelName) {
			Channels[channelName].TopicRestrictions = true;
		}
};

void	createChannel(string av[2], Channel &ch, string username, int fd);