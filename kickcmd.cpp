#include"kickcmd.hpp"
#include<sstream>

KickCmd::KickCmd(){}

KickCmd::~KickCmd(){}

//hadchi mzl 5asso ntchiki lpermesion d client li msendi request o nsendi response


//KICK <channel> <user> <comment>
void KickCmd::kick(std::string av[2], Channel &ch){
    std::string channel;
    std::string user;
   // std::string comment;

    channel = av[0];
    user = av[1];
    if(channel.empty()||user.empty()){
        //ghanssendi l user li sift request message bili lcommand machi hya hadik rh 5asso user o channel yd5lhom
        std::cout<<"Invalid command"<<std::endl;
        return;
    }
    std::vector<std::string>::iterator it;
    it = std::find(ch.getChannelNames().begin(),ch.getChannelNames().end(),channel);
    if(it==ch.getChannelNames().end()){
        std::cout<<"Channel does not exist"<<std::endl;
               //ghanssendi l user li sift request message bini makaynach had channel
        return;
    }
    //check if user is in the channel
     std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1==ch.getUserList(channel).end()){
        std::cout<<"User is not in the channel"<<std::endl;
       //ghanssendi l user li sift request message bini makaynch had luser f channel
        return;
    }
    else
    {
        ch.removeUser(channel,user);
        std::cout<<"Kicked "<<user<<" from "<<channel;
    }
}