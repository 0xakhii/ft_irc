#pragma once

#include "Server.hpp"
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

class Channel {
	private:
		struct ChannelData {
			string topic;
			set<string> operators;
			vector<string> userList;
		};
		map<string, ChannelData> Channels;
	public:
		bool hasChannel(const string& channelName) const {
			return Channels.count(channelName) > 0;
		}
		bool addChannel(const string& channelName) {
			if (hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].topic = "topic";
			Channels[channelName].operators.insert("username");
			Channels[channelName].userList.push_back("username");
			return true;
		}
		map<string, ChannelData> getChannel(const string& channelName) {
			if (!hasChannel(channelName)) {
			}
			return Channels;
		}
		bool removeChannel(const string& channelName) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels.erase(channelName);
			return true;
		}
		bool setTopic(const string& channelName, const string& newTopic) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].topic = newTopic;
			return true;
		}
		string getTopic(const string& channelName) const {
			if (!hasChannel(channelName)) {
				return "";
			}
			return Channels.at(channelName).topic;
		}
		bool addUser(const string& channelName, const string& username) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].userList.push_back(username);
			return true;
		}
		bool removeUser(const string& channelName, const string& username) {
			if (!hasChannel(channelName)) {
				return false;
			}
			ChannelData& channelData = Channels[channelName];
			vector<string>::iterator it = find(channelData.userList.begin(), channelData.userList.end(), username);
			if (it != channelData.userList.end()) {
				channelData.userList.erase(it);
				return true;
			}
			return false;
		}
		bool isOperator(const string& channelName, const string& username) const {
			if (!hasChannel(channelName)) {
				return false;
			}
			return Channels.at(channelName).operators.count(username) > 0;
		}
		bool addOperator(const string& channelName, const string& username) {
			if (!hasChannel(channelName)) {
				return false;
			}
			Channels[channelName].operators.insert(username);
			return true;
		}
		bool removeOperator(const string& channelName, const string& username) {
			if (!hasChannel(channelName)) {
				return false;
			}
			return Channels.at(channelName).operators.erase(username) > 0;
		}
		vector<string> getUserList(const string& channelName) const {
			if (!hasChannel(channelName)) {
				return vector<string>();
			}
			return Channels.at(channelName).userList;
		}
		vector<string> getChannelNames() const {
			vector<string> channelNames;
			for (map<string, ChannelData>::const_iterator it = Channels.begin(); it != Channels.end(); ++it) {
				channelNames.push_back(it->first);
			}
			return channelNames;
		}
};
