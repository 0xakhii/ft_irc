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
			bool InviteOnly;
			bool TopicRestrictions;
			string ChannelKey;
			bool isKeyRequired;
		};
		map<string, ChannelData> Channels;
	public:
		std::map<std::string,int> allclient;
		bool hasChannel(const string& channelName) const {
			return Channels.count(channelName) > 0;
		}
		bool addChannel(string& channelName, string username, int fd) {
			std::pair<map<string, ChannelData>::iterator, bool> result = Channels.insert({channelName, ChannelData{}});
			if (!result.second) {
				return false;
			}
			if(channelName[channelName.length()-1] == '\n')
				channelName.pop_back();
			result.first->second.name = channelName;
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
		string getTopic(const string& channelName) const {
			if (!hasChannel(channelName))
				return "";
			return Channels.at(channelName).topic;
		}
		bool addUser(const string& channelName, const string& username, const string& nickname, int fd) {
			Channels[channelName].userList[username] = fd;
			string toSend = ":localhost 353 " + nickname + " = " + channelName + " :@" + Channels.at(channelName).operators.begin()->c_str() + " ";
			const map<string, int>& userList = Channels.at(channelName).userList;
			for (map<string, int>::const_iterator it = userList.begin(); it != userList.end(); ++it) {
				if (nickname == it->first)
					continue;
				else
					toSend += it->first + " ";
			}
			toSend += "\r\n";
			string first = ":" + nickname + "!WEBSERV@localhost JOIN :" + channelName + "\r\n";
			string second = ":locahost 332 " + nickname + " " + channelName + " :No topic is set\r\n";
			string third = ": 333 " + nickname + " " + channelName + " " + nickname + "\r\n" ;
			string last = ":localhost 366 " + nickname + " #" + channelName + " :End of /NAMES list.\r\n";
			send(fd, first.c_str(), first.size(), 0);
			send(fd, second.c_str(), second.size(), 0);
			send(fd, third.c_str(), third.size(), 0);
			send(fd, toSend.c_str(), toSend.size(), 0);
			send(fd, last.c_str(), last.size(), 0);
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
		void broadcastMessage(const string& channelName, const string& message, int fd, string nickname) {
			if (!hasChannel(channelName)) {
				string toSend = string(ERR) + "Channel does not exist\n" + RESET;
				send(fd, toSend.c_str(), toSend.size(), 0);
				return;
			}
			string toSend = ":" + nickname + " PRIVMSG " + channelName + " :" + message + "\r\n";
			const map<string, int>& userList = Channels.at(channelName).userList;
			for (map<string, int>::const_iterator it = userList.begin(); it != userList.end(); ++it) {
				if (fd != it->second){
					send(it->second, toSend.c_str(), toSend.size(), 0);
				}
			}
		}
		void removeInviteOnly(const string& channelName) {
			Channels[channelName].InviteOnly = false;
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
		void setInviteOnly(const string& channelName) {
			Channels[channelName].InviteOnly = true;
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

void	createChannel(string av[2], Channel &ch, string username, string nickname, int fd);