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
    std::map<std::string,int> user_list = ch.getUserList(channel);
    invited_fd = findUserFdByUsername(user_list, user);
    std::cout<<"Kicked fd:--->>>> "<<invited_fd<<std::endl;
    if(invited_fd == -1){
        std::cout<<"User not found"<<std::endl;
        send_response(fd, "----User not found!----\n");
        return;
    }
    if(ch.isOperator(channel,client_name)==false){
        std::cout<<"You are not an operator"<<std::endl;
        send_response(fd, "----You are not an operator!----\n");
        return;
    }
    std::vector<std::string>::iterator it;
    it = std::find(ch.getChannelNames().begin(),ch.getChannelNames().end(),channel);
    if(it==ch.getChannelNames().end()){
        std::cout<<"Channel not found"<<std::endl;
        send_response(fd, "----Channel does not exist!----\n");
        //ghanssendi l user li sift request message bini makaynach had channel
        return;
    }
    std::map<std::string,int>::iterator it1;
    it1 = ch.getUserList(channel).find(user);
    if(it1!=ch.getUserList(channel).end()){
        std::cout<<"User is already in the channel"<<std::endl;
        send_response(fd, "----User is already in the channel!----\n");     
        //ghanssendi l user li sift request message bini luser kayn f channel
        return;
    }
    else
    {
        std::cout<<"Invited "<<user<<" to "<<channel;
        ch.addinviteeduser(channel,user);
        send_response(fd, "----User invited!----\n");
        send_response(invited_fd, "----You have been invited to the channel!----\n");
        //ghanssendi invite l user
    }
}