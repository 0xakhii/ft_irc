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
        //ghanssendi l user li sift request message bili lcommand machi hya hadik rh 5asso user o channel yd5lhom
        std::cout<<"Invalid command"<<std::endl;
        send_response(fd, "----Invalid command!----\n");
        return;
    }
    if(channel[0] == '#')
        channel = channel.substr(1);
    else{
        std::cout<<"Invalid command"<<std::endl;
        send_response(fd, "----Invalid channel name!----\n");
        return;
    }
    std::vector<std::string>::iterator it;
    it = std::find(ch.getChannelNames().begin(),ch.getChannelNames().end(),channel);
    if(it==ch.getChannelNames().end()){
        std::cout<<"Channel not found"<<std::endl;
        send_response(fd, nosuchchannel(channel));
        //ghanssendi l user li sift request message bini makaynach had channel
        return;
    }
    std::map<std::string,int> user_list = ch.getUserList(channel);
    std::map<std::string,int>::iterator itt2;
    itt2 = ch.allclient.begin();
    for(itt2;itt2!=ch.allclient.end();itt2++){
        if(itt2->first == user){
            invited_fd = itt2->second;
            break;
        }
    }
    if(invited_fd == -1){
        send_response(fd, nosuchuser(user));
        //ghanssendi l user li sift request message bini luser makaynach
        return;
    }
    if(ch.isOperator(channel,client_name)==false){
        std::cout<<"You are not an operator"<<std::endl;
        send_response(fd, notadmin(client_name,channel));
        return;
    }
    
    std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1!=ch.getUserList(channel).end()){
        std::cout<<"User is already in the channel"<<std::endl;
        send_response(fd, useronchannel(user,channel));     
        //ghanssendi l user li sift request message bini luser kayn f channel
        return;
    }
    else
    {
        std::cout<<"Invited "<<user<<" to "<<channel;
        ch.addinviteeduser(channel,user);
       // send_response(fd, "----User invited!----\n");
        send_response(invited_fd, Rpl_inviting(client_name,channel,user));
        //ghanssendi invite l user
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
    return(": 482 " + source_nick + " :You're not channel operator\r\n");
}

std::string Invitecmd::Rpl_inviting(std::string source_nick,std::string channel_name,std::string invited_nick){
    return(":" + source_nick + " INVITE " + invited_nick + " " + channel_name + "\r\n");
}

std::string Invitecmd::useronchannel(std::string invited_nick,std::string channel_name){
    return(": 443 " + invited_nick + " " + channel_name + " :is already on channel\r\n");
}