#include"Invitecmd.hpp"



//hadchi mzl 5asso lkhdma fih ghi lbasique



Invitecmd::Invitecmd(){}

Invitecmd::~Invitecmd(){}

void Invitecmd::invite(std::string av[2], Channel &ch){
    std::string channel;
    std::string user;
    
    user = av[0];
    channel = av[1];
    if(channel.empty()||user.empty()){
        //ghanssendi l user li sift request message bili lcommand machi hya hadik rh 5asso user o channel yd5lhom
        std::cout<<"Invalid command"<<std::endl;
        return;
    }
    std::vector<std::string>::iterator it;
    it = std::find(ch.getChannelNames().begin(),ch.getChannelNames().end(),channel);
    if(it==ch.getChannelNames().end()){
        std::cout<<"Channel not found"<<std::endl;
        //ghanssendi l user li sift request message bini makaynach had channel
        return;
    }
    std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1!=ch.getUserList(channel).end()){
        std::cout<<"User is already in the channel"<<std::endl;
        //ghanssendi l user li sift request message bini luser kayn f channel
        return;
    }
    else
    {
        std::cout<<"Invited "<<user<<" to "<<channel;
        //ghanssendi invite l user
    }
}