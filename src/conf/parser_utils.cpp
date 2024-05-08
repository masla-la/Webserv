#include "../../inc/webserv.hpp"

bool	ft_envcmp(char *env, std::string line)
{
	int i;

	i = 0;
	while (line[i] && env[i] == line[i + 1])
		i++;
	if (env[i] == '=')
		return true;
	return false;
}

std::string	ft_strndup(char *env)
{
	std::string	dest;

	dest = env;
	dest.erase(0, dest.find('=') + 1);
	return dest;
}

std::string	returnEnv(char **env, std::string line)
{
	std::string	dest;

	for (int i = 0; env[i]; i++)
		if (ft_envcmp(env[i], line))
			return (ft_strndup(env[i]));
	return "default";
}

void	parse_arg(int ac, char **av)
{
	if (ac != 2)
		throw ArgError();

	std::ifstream	file(av[1]);

	if (!file)
		throw ArgError();
	else
	{
		std::string	name = av[1];
		if (name.find(".conf") < 0)
			throw ArgError();	
	}
}

void	parser(int ac, char **av, Config &conf, char **env)
{
	parse_arg(ac, av);
	conf.readFd(av[1]);
	conf.checkConfig(env);
}

bool	isdirective(std::string line, std::vector<std::string> dir)
{
	for (unsigned int i = 0; i < dir.size(); i++)
	{
		if (line == dir[i])
			return true;
	}
	return false;
}