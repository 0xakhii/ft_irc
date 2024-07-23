#include"Server.hpp"

void Server::irc_bot(Client &client,std::string arg)
{
        if(arg.empty())
        {   
            std::vector<std::string> possible_qes;
            std::string res = "Welcome " + client.getNickname() + " to our IRC Server \r\n";
            send(client.getFd(),res.c_str(),res.size(),0);
            std::string res1 = "The is the available question you can ask me please set the number of the question\n";
            std::string res2 = "1- Who is the creator of this server?\n";
            std::string res3 = "2- What is the commands that you provide?\n";
            std::string res4 = "3- What is the available channels?\r\n";
            res = res1 + res2 + res3 + res4;
            send(client.getFd(),res.c_str(),res.size(),0);
            return;
        }
        else
        {
            if(arg == "1")
            {
                std::string res = "The creators of this server are : MOHAMED ET-TAHERY, OMAR JAMAL, REDA BOUISSALI\r\n";
                send(client.getFd(),res.c_str(),res.size(),0);
                return;
            }
            else if(arg == "2")
            {
                std::string res = "The commands that you can use are : \r\n";
                std::string res1 = "1- /join <channel> : to join a channel\n";
                std::string res2 = "2- /kick <channel> <nickname> : to kick a member from a channel\n";
                std::string res3 = "3- /invite <nickname> <channel> : to invite a member to a channel\n";
                std::string res4 = "4- /topic <channel> <topic> : to set a topic to a channel\n";
                std::string res5 = "5- /mode <channel> <mode> : to set a mode to a channel\r\n";
                res = res + res1 + res2 + res3 + res4 + res5;
                send(client.getFd(),res.c_str(),res.size(),0);
                return;
            }
            else if(arg == "3")
            {
                std::string res = "The available channels are : \r\n";
                std::vector<std::string> chs = ch.getChannelNames();
                for(size_t i = 0; i < chs.size(); i++)
                {
                    res = res + chs[i] + "\n";
                }
                res = res + "\r\n";
                send(client.getFd(),res.c_str(),res.size(),0);
                return;
            }
            else
            {
                std::string res = "Invalid question,Please set the right number of the question\r\n";
                send(client.getFd(),res.c_str(),res.size(),0);
                return;
            }
        }
    return;
}
