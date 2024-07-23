#include"Invitecmd.hpp"



//hadchi mzl 5asso lkhdma fih ghi lbasique



Invitecmd::Invitecmd(){
    client_name = "";
    invited_fd = -1;
}

Invitecmd::~Invitecmd(){}

void Invitecmd::invite(std::string av[2], Channel &ch,int fd){
    std::string channel;
    std::string user;
    
    user = av[0];
    channel = av[1];
    if(channel.empty()||user.empty()){
        send_response(fd, notenoughparams());
        return;
    }
    if(channel[0] != '#'){
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
    std::map<std::string,int> user_list = ch.getUserList(channel);
    std::map<std::string,int>::iterator itt2;
    itt2 = ch.allclient.begin();
    for(;itt2!=ch.allclient.end();itt2++){
        if(itt2->first == user){
            invited_fd = itt2->second;
            break;
        }
    }
    if(invited_fd == -1){
        send_response(fd, nosuchuser(user));
        return;
    }
    if(ch.isOperator(channel,client_name)==false){
        send_response(fd, notadmin(client_name,channel));
        return;
    }
    
    std::map<std::string,int>::iterator it1;
    it1 = ch.getChannels(channel).find(user);
    if(it1!=ch.getUserList(channel).end()){
        send_response(fd, useronchannel(user,channel));     
        return;
    }
    else
    {
        ch.addinviteeduser(channel,user);
        send_response(invited_fd, Rpl_inviting(client_name,channel,user));
    }
}

std::string Invitecmd::notenoughparams(){
    return(": 461 INVITE Not enough parameters\r\n");
}

std::string Invitecmd::nosuchchannel(std::string channel_name){
    return(": 403 " + channel_name + " :No such channel\r\n");
}

std::string Invitecmd::nosuchuser(std::string invited_nick){
    return(": 401 " + invited_nick + " :No such nick/channel\r\n");
}

std::string Invitecmd::notadmin(std::string source_nick,std::string channel_name){
    (void)channel_name;
    return(": 482 " + source_nick + " :You're not channel operator\r\n");
}

std::string Invitecmd::Rpl_inviting(std::string source_nick,std::string channel_name,std::string invited_nick){
    return(":" + source_nick + " INVITE " + invited_nick + " " + channel_name + "\r\n");
}

std::string Invitecmd::useronchannel(std::string invited_nick,std::string channel_name){
    return(": 443 " + invited_nick + " " + channel_name + " :is already on channel\r\n");
}