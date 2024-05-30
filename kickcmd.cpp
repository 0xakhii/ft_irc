#include"kickcmd.hpp"
#include<sstream>

KickCmd::KickCmd(){
    client_name = "";
    kicked_fd = -1;
}

KickCmd::~KickCmd(){}

//hadchi mzl 5asso ntchiki lpermesion d client li msendi request o nsendi response

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
void KickCmd::kick(std::string av[2], Channel &ch,int fd){
    std::string channel;
    std::string user;
    // std::string comment;

    channel = av[0];
    user = av[1];
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
    std::map<std::string,int> user_list = ch.getUserList(channel);
    kicked_fd = findUserFdByUsername(user_list, user);
    std::cout<<"Kicked fd:--->>>> "<<kicked_fd<<std::endl;
    if(kicked_fd == -1){
        std::cout<<"User not found"<<std::endl;
        send_response(fd, "----User not found!----\n");
        return;
    }
    //exit(0);
    std::cout<<"i am here now"<<std::endl;
    
    if(ch.isOperator(channel,client_name)==false){
        std::cout<<"You are not an operator"<<std::endl;
        send_response(fd, "----You are not an operator!----\n");
        return;
    }
    std::vector<std::string>::iterator it;
    it = std::find(ch.getChannelNames().begin(),ch.getChannelNames().end(),channel);
    if(it==ch.getChannelNames().end()){
        std::cout<<"Channel does not exist"<<std::endl;
        send_response(fd, "----Channel does not exist!----\n");
               //ghanssendi l user li sift request message bini makaynach had channel
        return;
    }
    //check if user is in the channel
     std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1==ch.getUserList(channel).end()){
        std::cout<<"User is not in the channel"<<std::endl;
        send_response(fd, "----User is not in the channel!----\n");
       //ghanssendi l user li sift request message bini makaynch had luser f channel
        return;
    }
    else
    {
        //std::cout<<"Kicked fd: "<<aa<<std::endl;
        ch.removeUser(channel,user);
        send_response(fd, "----User kicked from the channel!----\n");
       // std::cout<<"---->"<<it1->second<<std::endl;
        send_response(kicked_fd, "----You have been kicked from the channel!----\n");
        std::cout<<"Kicked "<<user<<" from "<<channel;
    }
}

void send_response(int fd, std::string response){
    if(send(fd, response.c_str(), response.size(), 0) < 0)
        std::cout<<"SEND FAILED!"<<std::endl;
}