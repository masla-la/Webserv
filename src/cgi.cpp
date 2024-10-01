#include "../inc/webserv.hpp"

bool	is_cgi(std::string url)
{
	if (url.find("/cgi-bin/"))
			return false;
	return true;
}

std::string	check_script(std::string path)
{
	std::string	tmp = path;

	tmp.erase(0, tmp.find_last_of('.') + 1);
	if (tmp == "py")
		return "/python3";
	return NULL;
}

std::string	cgi_ex(std::string url, std::string query, Server &server, char **env)
{
	//Modificar GET y POST
	std::string	path = server.getRoot() + url;
	std::string	envPath;
	std::string	type;
	path.erase(0, 2);

	if (!access(path.c_str(), X_OK))
	{
		std::cout << "Error: access error" << std::endl;
		return "";
	}

	envPath = returnEnv(env, "PATH");
	while (envPath.find(':') < envPath.size())
		envPath.replace(envPath.find(':'), 1, " ");

	std::stringstream	ss(envPath);
	std::string	pathTmp;
	std::vector<const char*> av;
	std::vector<std::string> ex;

	if ((type = check_script(path)).empty())
		return "";

	while (ss >> pathTmp)
		ex.push_back(pathTmp + type);

	av.push_back(type.c_str());
	av.push_back(path.c_str());
	query.erase(0, 1);
	if (query.empty())
		return "";
	av.push_back(query.c_str());
	av.push_back(NULL);

	std::string	req;
	pid_t	pid;
	int		pipefd[2];

	pipe(pipefd);

	pid = fork();
	if (pid == 0)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		for (std::vector<std::string>::iterator it = ex.begin(); it != ex.end(); it++)
			execve(const_cast<const char*>((*it).c_str()), const_cast<char * const*>(av.data()), NULL);
		exit (1);
	}
	else
	{
		close(pipefd[1]);
		if (waitpid(pid, 0, 0))
		{

			char		buff[128];
			size_t		i = 0;

			while((i = read(pipefd[0], buff, sizeof(buff) - 1)) > 0)
			{
				buff[i] = '\0';
				for(size_t n = i; n < sizeof(buff); n++)
					buff[n] = '\0';
				req += buff;
			}
		}
		close(pipefd[0]);
	}
	return req;
}
