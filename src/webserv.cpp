#include "../inc/webserv.hpp"
#include "../inc/ServerManager.hpp"
#include "../inc/conf/Config.hpp"

size_t	ft_stoi(std::string str)//
{
	int	i = -1;
	int dest = 0;

	while (++i < str.size())
	{
		if (str[i] < '0' || str[i] > '9')
			throw "Number error{ ft_stoi }";//Error ,no es numero
		dest = dest * 10 + (str[i] - '0');
	}
	return  dest;
}

uint32_t to_uint32_t( const std::string & ip_address )
{
    const unsigned bits_per_term = 8 ;
    const unsigned num_terms = 4 ;

    std::istringstream ip(ip_address) ;
    uint32_t packed = 0 ;

    for (unsigned i = 0 ;  i < num_terms; ++i)
    {
        unsigned term ;
        ip >> term ;
        ip.ignore() ;

        packed += term << (bits_per_term * (num_terms - i - 1)) ;
    }

    return packed ;
}

int main(int ac, char **av, char **env)
{
	Config			conf;
	ServerManager	manager;
	
	try
	{
		parser(ac, av, conf, env);
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	manager.InitServer(conf.getServ());
	while (1)
	{
		manager.waitClient();
		manager.acceptClient();
		manager.handle_request();
	}
	return 0;
}