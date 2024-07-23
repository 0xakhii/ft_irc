#include"kickcmd.hpp"
#include<sstream>

KickCmd::KickCmd(){
    client_name = "";
    kicked_fd = -1;
}

KickCmd::~KickCmd(){}

void splitkickargs(std::string& str,std::vector<std::string>& args) {
	std::string word;
	for (size_t i = 0; i < str.size()-2; i++) {
		if (str[i] == ' ') {
			args.push_back(word);
			word.clear();
		}
		else
			word += str[i];
	}
}

int findUserFdByUsername(const std::map<std::string, int>& userMap,  std::string& username) {
    std::map<std::string, int>::const_iterator it = userMap.begin();
    while (it != userMap.end()) {
        if (it->first == username) {
            return it->second;
        }
        it++;
    }
    return -1;
}

void KickCmd::kick(std::string full_args, Channel &ch,int fd){
    std::string channel;
    std::string user;
    std::string comment;
    splitkickargs(full_args, args);
    if(args.size() < 2){
        send_response(fd, notenoughparams());
        return;
    }
    else
    {
        if(args.size()== 3)
        {
            comment = args[2];
            channel = args[0];
            user = args[1];
        }
        else
        {
            channel = args[0];
            user = args[1];
            comment = "";
        }
    }
    if(channel.empty()||user.empty()||channel[0] != '#'){
        send_response(fd, nosuchchannel(channel));
        return;
    }
    std::vector<std::string>vec1;
    vec1 = ch.getChannelNames();
    std::vector<std::string>::iterator it;
    it = std::find(vec1.begin(),vec1.end(),channel);
    if(it==vec1.end()){
        send_response(fd, nosuchchannel(channel));
        return;
    }
    std::map<std::string,int> user_list = ch.getChannels(channel);
    kicked_fd = findUserFdByUsername(user_list, user);
    if(kicked_fd == -1){
        send_response(fd, nosuchuser(user));
        return;
    }
    if(fd == kicked_fd){
        send_response(fd, kickyourself(client_name));
        return;
    }
    if(ch.isOperator(channel,client_name)==false){
        send_response(fd, notadmin(client_name,channel));
        return;
    }
    std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1==ch.getUserList(channel).end()){
        send_response(fd, nosuchuser(user));
        return;
    }
    else
    {
        std::map<std::string,int>::iterator it3;
        it3 = user_list.begin();
        for(;it3!=user_list.end();it3++)
        {
            if(it3->first==user)
            {
                send_response(kicked_fd, ":" + client_name + "!~" + user + "@localhost KICK " + channel + " " + user + " :You have been kicked\r\n");
                ch.removeUser(channel,user);
                break;
            }
        }
    }
}

void send_response(int fd, std::string response){
    if(send(fd, response.c_str(), response.size(), 0) < 0)
        std::cout<<"SEND FAILED!"<<std::endl;
}
void sendresp_all(int fd,std::string kicked_nick,std::string source_nick,string channel_name,std::string comment)
{
    std::string res = ":"+source_nick+" KICK "+channel_name+" "+ kicked_nick;
    if(comment!="")
        res+=" :"+comment;
    if(send(fd,(res+"\r\n").c_str(), res.length(), 0) < 0)
        std::cout<<"SEND FAILED!"<<std::endl;
}

std::string KickCmd::notenoughparams(){
    return ":461 KICK :Not enough parameters\r\n";
}

std::string KickCmd::nosuchchannel(std::string channel_name){
    return ":403 "+channel_name+" :No such channel\r\n";
}

std::string KickCmd::nosuchuser(std::string kicked_nick){
    std::string res =":401 "+kicked_nick+" :No such nick/channel\r\n";
    return res;
}

std::string KickCmd::notadmin(std::string source_nick,std::string channel_name){
    (void)source_nick;
    return(":localhost 482 " + channel_name + " :You're not channel operator\r\n");
}

std::string KickCmd::kickyourself(std::string source_nick){
    return(":488 " + source_nick + " :You can not kick yourself\r\n");
}