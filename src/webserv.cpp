#include "../inc/webserv.hpp"
#include "../inc/ServerManager.hpp"
#include "../inc/conf/Config.hpp"

ServerManager	manager;

size_t	ft_stoi(std::string str)//
{
	size_t	i = -1;
	size_t dest = 0;

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

void	ft_sig(int signal)
{
	(void)signal;
	std::vector<Client>	client = manager.getClient();
	std::vector<Server> server = manager.getServer();
	for (size_t i = 0; i < server.size(); i++)
		close(server[i].getSock());
	for (size_t i = 0; i < client.size(); i++)
		close(client[i].getSock());
	exit(0);
}

int main(int ac, char **av, char **env)
{
	Config			conf;
	//ServerManager	manager;
	
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
	signal(SIGINT, ft_sig);
	while (1)
	{
		manager.acceptClient();
		manager.waitClient();
		manager.handle_request();
	}
	return 0;
}