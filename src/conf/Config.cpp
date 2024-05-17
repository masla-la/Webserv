#include "../../inc/conf/Config.hpp"

Config::Config(void)
{
	_directives.push_back("listen");
	_directives.push_back("server_name");
	_directives.push_back("allow_methods");
	_directives.push_back("root");
	_directives.push_back("error_pages");
	_directives.push_back("index");
	_directives.push_back("client_max_body_size");
	_directives.push_back("host");
	_directives.push_back("dir_listing");
	_directives.push_back("location");
}

Config::~Config(void)
{
}


void	Config::readFd(std::string namefd)
{
	std::ifstream	file(namefd.c_str());
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
	size_t			n = 0;

	for(size_t i = 0; i < _fd.size() + 1; i++)
	{
		if (n == 0 && _fd[i] == "server")
			n++;
		else if (i == _fd.size() || _fd[i] == "server")
		{
			_server.push_back(server);
			if (i == _fd.size())
				break ;
			Server	newServer;
			server = newServer;
		}
		else
		{
			line = _fd[i];
			conf = _fd[i];
			if (_fd[i].size() == _fd[i].find(' ') + 1 || _fd[i].find(' ') == std::string::npos)
				throw ConfError();
			line.erase(_fd[i].find(' '), conf.size() - _fd[i].find(' '));
			if (!isdirective(line, _directives))
				throw BadConfig();
			conf.erase(0, _fd[i].find(' ') + 1);
			if (line == "location")
				i = parseLocation(i + 1, conf, server);
			else
				checkDirective(line, conf, server, env);
		}
	}
}

void	Config::checkDirective(std::string line, std::string conf, Server & server, char **env)
{
	for (size_t i = 0; i < _directives.size(); i++)
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
					server.setMethod(conf);
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

				case 8:
					server.setListing(conf);
					break ;

				default:
					break;
			}
	}
}

size_t	Config::parseLocation(size_t n, std::string dir, Server server)
{
	Location	loc;

	loc.setDir(dir);
	std::vector<std::string>	direct = loc.getDirectives();
	std::string					line;
	std::string					conf;

	while (n <= _fd.size() && _fd[n] != "server")
	{
		line = _fd[n];
		conf = _fd[n];
		if (_fd[n].size() == _fd[n].find(' ') + 1 || _fd[n].find(' ') == std::string::npos)
			throw ConfError();
		line.erase(_fd[n].find(' '), conf.size() - _fd[n].find(' '));
		if (line == "locate")
			return n - 1;
		if (!isdirective(line, direct))
			throw BadConfig();
		conf.erase(0, _fd[n].find(' ') + 1);

		for (size_t i = 0; i < direct.size(); i++)
		{
			if (direct[i] == line)
			{
				switch (i)
				{
					case 0:
						loc.setRoot(conf);
						break ;

					case 1:
						loc.setIndex(conf);
						break ;

					case 2:
						loc.setMethods(conf);
						break ;

					case 3:
						loc.setListing(conf);
						break ;
						
					default:
						break ;
				}
			}
		}
		n++;
	}
	server.setLocation(loc);
	return (n - 1);
}

std::vector<Server>	Config::getServ(void)
{
	return _server;
}
