#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <set>
#include"kickcmd.hpp"

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
		map<string,int> allclient;
		map<string,int> getChannels(string channel_name){
			map<string,int> res;
			for (map<string, ChannelData>::iterator it = Channels.begin(); it != Channels.end(); it++)
			{
				if (it->first.find(channel_name) != string::npos)
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
			pair<map<string, ChannelData>::iterator, bool> result = Channels.insert({channelName, ChannelData{}});
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
		void send_responsed(int fd, string response){
    	if(send(fd, response.c_str(), response.size(), 0) < 0)
        cout<<"SEND FAILED!"<<endl;
		}

		bool addUser(const string& channelName, const string& username, int fd) {

			Channels[channelName].userList[username] = fd;
			map<string, int>::iterator it;
			string member_str = ":localhost 353 " + username + " = " + channelName + " :@" + Channels[channelName].admin + " ";
    		for (it = Channels[channelName].userList.begin(); it != Channels[channelName].userList.end(); ++it)
    		{
				if (it->first == Channels[channelName].admin)
					continue;
				else
    		    	member_str += it->first + " ";
    		}
    		member_str += "\r\n";
			send(fd, member_str.c_str(), member_str.size(), 0);
			send_responsed(fd, ":" + username + "!" + username + "@localhost JOIN :" + channelName + "\r\n");
			send_responsed(fd, ":localhost 366 " + username + " " + channelName + " :End of /NAMES list.\r\n");
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
		bool addOperator(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return false;
			}
			Channels[channelName].operators.insert(nickname);
			return true;
		}
		bool removeOperator(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return false;
			}
			return Channels.at(channelName).operators.erase(nickname) > 0;
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
		void removeInviteOnly(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].InviteOnly = false;
			string str = ":" + nickname+ " MODE " + channelName + " -i\r\n";
			send(fd, str.c_str(), str.size(), 0);
		}
		bool getInviteOnly(const string& channelName) const {
			return Channels.at(channelName).InviteOnly;
		}
		bool isInvited(const string& channelName, const string& username) const {
			return find(Channels.at(channelName).invitelist.begin(), Channels.at(channelName).invitelist.end(), username) != Channels.at(channelName).invitelist.end();
		}
		void removeUserLimit(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].userLimit = -1;
		}
		void removeChannelKey(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].invitelist.clear();
			Channels[channelName].isKeyRequired = false;
		}
		bool isKeyRequired(const string& channelName) const {
			return Channels.at(channelName).isKeyRequired;
		}
		string getChannelKey(const string& channelName) const {
			return Channels.at(channelName).ChannelKey;
		}
		void removeTopicRestrictions(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].TopicRestrictions = false;
		}
		void setInviteOnly(const string& channelName,bool inviteOnly,string nickname,int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].InviteOnly = inviteOnly;
			string str = ":" + nickname+ " MODE " + channelName + " +i\r\n";
			send(fd, str.c_str(), str.size(), 0);
		}
		void setUserLimit(const string& channelName, int limit, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].userLimit = limit;
		}
		int getUserLimit(const string& channelName) const {
			return Channels.at(channelName).userLimit;
		}
		void setChannelKey(const string& channelName, const string& key, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].ChannelKey = key;
			Channels[channelName].isKeyRequired = true;
		}
		void setTopicRestrictions(const string& channelName, const string& nickname, int fd) {
			if (!isOperator(channelName, nickname)){
				string toSend = ":localhost 482 " + channelName + " :You're not channel operator\r\n";
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			Channels[channelName].TopicRestrictions = true;
		}
};

void	createChannel(string av[2], Channel &ch, string username, int fd);
std::string relaymsg(string av[10000]);