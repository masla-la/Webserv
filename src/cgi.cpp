#include "../inc/webserv.hpp"

bool	is_cgi(std::string url)
{
	if (url.find("/cgi-bin/"))
			return false;
	return true;
}

void	cgi_ex(std::string url, std::string query, Client &client, Server &server)
{
	(void)client;
	std::string fd = url;
	std::string	tmp = query;
	
	std::string	path = server.getRoot() + url;
	path.erase(0, 2);

	if (!access(path.c_str(), X_OK))
		std::cout << "Error: access error" << std::endl;

	char	*av[4];

	//
	av[0] = strdup("/usr/bin/python3");
	av[1] = strdup(path.c_str());
	av[2] = strdup(query.c_str());
	av[3] = NULL;
	//

	std::cout << "123\n";
	pid_t	pid;
	pid = fork();
	if (pid == 0)
	{
		execve("/usr/bin/python3", av, NULL);
		exit (1);
	}
	waitpid(pid, 0, 0);
	delete av[1];
	delete av[2];
	delete av[3];
}
