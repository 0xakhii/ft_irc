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
    s.pass=av[2];
    s.be_ready_for_connection();
}