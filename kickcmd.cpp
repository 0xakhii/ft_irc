#include"kickcmd.hpp"
#include<sstream>

KickCmd::KickCmd(){
    client_name = "";
    kicked_fd = -1;
}

KickCmd::~KickCmd(){}

void splitkickargs(std::string& str,std::vector<std::string>& args) {
	std::string word;
	for (size_t i = 0; i < str.size(); i++) {
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
    username = username.substr(0, username.size()-1);
    while (it != userMap.end()) {
        std::cout<<"User: "<<it->first.size()<<" fd: "<<it->second<<std::endl;
        std::cout<<"Username  "<<(int)username[5]<<std::endl;
        if (it->first == username) {
            std::cout<<"i am here now______"<<std::endl;
            return it->second;
        }
        it++;
    }
    return -1;
}



//KICK <channel> <user> <comment>
void KickCmd::kick(std::string full_args, Channel &ch,int fd){
    std::string channel;
    std::string user;
    std::string comment;

    splitkickargs(full_args, args);
    //tantchecki wach ga3 les arguments d kick kaynin
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
    std::cout<<"Channel: "<<channel<<std::endl;
    std::cout<<"User: "<<user<<std::endl;
    // if(channel.empty()||user.empty()){
    //     std::cout<<"Invalid command"<<std::endl;
    //     send_response(fd, "----Invalid command!----\n");
    //     return;
    //}
    if(channel[0] == '#')
        channel = channel.substr(1);
    else{
        std::cout<<"Invalid command"<<std::endl;
        send_response(fd, "----Invalid channel name!----\n");
        return;
    }
    //tantchecki wach kayna had channel
    std::vector<std::string>::iterator it;
    it = std::find(ch.getChannelNames().begin(),ch.getChannelNames().end(),channel);
    if(it==ch.getChannelNames().end()){
        std::cout<<"Channel does not exist"<<std::endl;
        send_response(fd, nosuchchannel(channel));
        return;
    }
    std::map<std::string,int> user_list = ch.getUserList(channel);
    kicked_fd = findUserFdByUsername(user_list, user);
    std::cout<<"Kicked fd:--->>>> "<<kicked_fd<<std::endl;
    //tantchecki wach kayn had user f had channel
    if(kicked_fd == -1){
        std::cout<<"User not found"<<std::endl;
        send_response(fd, nosuchuser(user));
        return;
    }
    //tantchecki wach had user li mssift request bgha ykicki rasso
    if(fd == kicked_fd){
        std::cout<<"You can not kick yourself"<<std::endl;
        send_response(fd, kickyourself(client_name));
        return;
    }
    //tantchecki wach had user li mssift request admin
    if(ch.isOperator(channel,client_name)==false){
        std::cout<<"You are not an operator"<<std::endl;
        send_response(fd, notadmin(client_name,channel));
        return;
    }
    std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1==ch.getUserList(channel).end()){
        std::cout<<"User is not in the channel"<<std::endl;
        send_response(fd, nosuchuser(user));
        return;
    }
    else
    {
        //tankicki had luser mn had channel
        ch.removeUser(channel,user);
        std::map<std::string,int>::iterator it3;
        it3 = user_list.begin();
        for(it3;it3!=user_list.end();it3++)
            sendresp_all(kicked_fd,user,client_name,channel,comment);
        std::cout<<"Kicked "<<user<<" from "<<channel;
    }
}

void send_response(int fd, std::string response){
    if(send(fd, response.c_str(), response.size(), 0) < 0)
        std::cout<<"SEND FAILED!"<<std::endl;
}
void sendresp_all(int fd,std::string kicked_nick,std::string source_nick,string channel_name,std::string comment)
{
    std::string res = ":"+source_nick+" KICK #"+channel_name+" "+ kicked_nick;
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
    return ":401 "+kicked_nick+" :No such nick/channel\r\n";
}

std::string KickCmd::notadmin(std::string source_nick,std::string channel_name){
    return(":localhost 482 " + channel_name + " :You're not channel operator\r\n");
}

std::string KickCmd::kickyourself(std::string source_nick){
    return(":488 " + source_nick + " :You can not kick yourself\r\n");
}