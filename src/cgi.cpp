#include "../inc/webserv.hpp"

bool	is_cgi(std::string url)
{
	if (url.find("/cgi-bin/"))
			return false;
	return true;
}

std::string	cgi_ex(std::string url, std::string query, Client &client, Server &server)
{
	(void)client;
	std::string fd = url;
	std::string	tmp = query;
	
	std::string	path = server.getRoot() + url;
	path.erase(0, 2);

	if (!access(path.c_str(), X_OK))
		std::cout << "Error: access error" << std::endl;

	//---
	std::vector<const char*> av;

	av.push_back("/usr/bin/python3");
	av.push_back(path.c_str());
	query.erase(0, 1);
	av.push_back(query.c_str());
	av.push_back(NULL);
	//---

	std::string	req;
	pid_t	pid;
	int		pipefd[2];

	pipe(pipefd);

	pid = fork();
	if (pid == 0)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		dup2(pipefd[0], STDIN_FILENO);
		execve("/usr/bin/python3", const_cast<char * const*>(av.data()), NULL);
		exit (1);
	}
	else
	{
		close(pipefd[1]);
		waitpid(pid, 0, 0);

		char		buff[128];
		ssize_t		i = 0;

		while((i = read(pipefd[0], buff, sizeof(buff) - 1)) > 0)
		{
			buff[i] = '\0';
			req += buff;
		}
		//---
		close(pipefd[0]);
	}
	return req;
}
