#include"Server.hpp"

int main(int ac,char **av)
{
    if(ac!=3)
    {
        std::cout<<"invalide arguments";
        return 0;
    }
    
    Server s;
    s.port=std::atoi(av[1]);
    const std::string& str=av[2];
     if (str.find(' ') != std::string::npos) {
        std::cout << "please don't print a space in the password." << std::endl;
        return 0;
    }
    s.pass=av[2];
    s.be_ready_for_connection();
    return 0;
}