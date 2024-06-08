#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <set>
#include"kickcmd.hpp"

#define ERR "\033[1;31mError:\033[0;0m\n\033[1m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0;0m"

using namespace std;

class KickCmd;

class Channel {
	private:
		struct ChannelData {
			string name;
			string topic;
			set<string> operators;
			map<string, int> userList;
			int userLimit;
			vector<string> invitelist;
		};
		map<string, ChannelData> Channels;
	public:
		bool hasChannel(const string& channelName) const {
			return Channels.count(channelName) > 0;
		}
		bool addChannel(const string& channelName, string username, int fd) {
			cout << channelName << " ---\n";
			std::pair<map<string, ChannelData>::iterator, bool> result = Channels.insert({channelName, ChannelData{}});
			if (!result.second) {
				return false;
			}
			result.first->second.name = channelName;
			result.first->second.topic = "topic\n";
			result.first->second.operators.insert(username);
			result.first->second.userList[username] = fd;
			result.first->second.userLimit = -1;
			cout << "Channel Created: " << result.first->second.name << endl;
			cout << "OP USER: " << *result.first->second.operators.find(username) << endl;
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
			if (!isOperator(channelName, username))
				return false;
			else
				Channels[channelName].topic = newTopic;
			return true;
		}
		string getTopic(const string& channelName) const {
			if (!hasChannel(channelName))
				return "";
			return Channels.at(channelName).topic;
		}
		bool addUser(const string& channelName, const string& username, int fd) {
			Channels[channelName].userList[username] = fd;
			broadcastMessage(channelName, "has joind the channel\n", fd, username);
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
		bool isOperator(const string& channelName, const string& username) const {
			return Channels.at(channelName).operators.count(username) > 0;
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
				cout << ERR << "Invalid Channel Name\n" << RESET;
				return;
			}
			const map<string, int>& userList = Channels.at(channelName).userList;
			for (map<string, int>::const_iterator it = userList.begin(); it != userList.end(); ++it) {
				cout << "Broadcasting message to user: " << it->first << endl;
				string toSend = string(YELLOW) + "#" + channelName + ":\n" + username + ": " + RESET + message;
				if (fd != it->second){
					send(it->second, toSend.c_str(), toSend.size(), 0);
				}
			}
		}
		bool removeInviteOnly(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].invitelist.clear();
			return true;
		}
		bool removeUserLimit(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].userLimit = -1;
			return true;
		}
		bool removeChannelKey(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].invitelist.clear();
			return true;
		}
		bool removeTopicRestrictions(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].invitelist.clear();
			return true;
		}
		bool setInviteOnly(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].invitelist.clear();
			return true;
		}
		bool setUserLimit(const string& channelName, int limit) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].userLimit = limit;
			return true;
		}
		bool setChannelKey(const string& channelName, const string& key) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].invitelist.clear();
			return true;
		}
		bool setTopicRestrictions(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].invitelist.clear();
			return true;
		}
};

void	createChannel(string arg, Channel &ch, string username, int fd);