#include "../../inc/conf/Config.hpp"

Config::Config(void)
{
	//_directives.push_back("server");
	_directives.push_back("listen");
	_directives.push_back("server_name");
	_directives.push_back("allow_methods");
	_directives.push_back("root");
	_directives.push_back("error_pages");
	_directives.push_back("index");
	_directives.push_back("client_max_body_size");
	_directives.push_back("host");//
	//_directives.push_back("location");
	//
}

Config::~Config(void)
{
}


void	Config::readFd(std::string namefd)
{
	std::ifstream	file(namefd);
	std::string 	out;

	while (std::getline(file, out))
	{
		unsigned int	len = out.length();
		
		for (unsigned int i = 0; i < len; i++)
		{
			if (out[i] != ' ')
			{
				_fd.push_back(out);
				break;
			}
		}
	}
	if (_fd.empty())
		throw EmptyFile();
}


void	Config::checkConfig(char **env)
{
	std::string		line;
	std::string		conf;
	Server			server;
	int				n = 0;

	for(unsigned int i = 0; i < _fd.size() + 1; i++)
	{
		if ( n == 0 && _fd[i] == "server")
			n++;
		else if (i == _fd.size() || _fd[i] == "server")
		{
			_server.push_back(server);
			Server	newServer;
			server = newServer;
		}
		else
		{
			line = _fd[i];
			conf = _fd[i];
			if (_fd[i].size() == _fd[i].find(' ') + 1 || _fd[i].find(' ') == -1)
				throw ConfError();
			line.erase(_fd[i].find(' '), conf.size() - _fd[i].find(' '));
			if (!isdirective(line, _directives))
				throw BadConfig();
			conf.erase(0, _fd[i].find(' ') + 1);
			if (line == "location")
				parseLocation(_fd,  server, env);//
			checkDirective(line, conf, server, env);
		}
	}
}

void	Config::checkDirective(std::string line, std::string conf, Server & server, char **env)
{
	//if (conf.empty())
	//	throw "Error: conf sin asignar";//no tiene conf la directiva
	for (int i = 0; i < _directives.size(); i++)
	{
		if (line == _directives[i])
			switch (i)
			{
				case 0:
					server.setPort(ft_stoi(conf));
					break;

				case 1:
					server.setServerName(conf, env);
					break;

				case 2:
					//allow_methods
					break;

				case 3:
					server.setRoot(conf, env);
					break;

				case 4:
					server.setErrorPages(conf);
					break;

				case 5:
					server.setIndex(conf);
					break;
			
				case 6:
					server.setMaxBody(ft_stoi(conf));
					break;

				case 7:
					server.setHost(conf);
					break;

				default:
					break;
			}
	}
}

void	Config::parseLocation(std::vector<std::string> fd, Server server, char **env)
{}

std::vector<Server>	Config::getServ(void)
{
	return _server;
}
