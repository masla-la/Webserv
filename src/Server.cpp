#include "../inc/Server.hpp"

Server::Server(void)
{
	_port = 8080;
	_server_name = "Default";
	_root = "./html";//
	_index = "index.html";
	_client_max_body_size = 1024;
	_host = "0.0.0.0";
}

Server::Server(const Server & obj)//
{
	*this = obj;
}

Server::~Server(void)
{}

Server	&Server::operator=(Server const &obj)
{
	_socket = obj._socket;
	_addr = obj._addr;
	_port = obj._port;

	_server_name = obj._server_name;
	_root = obj._root;
	_error_pages = obj._error_pages;
	_index = obj._index;
	_client_max_body_size = obj._client_max_body_size;
	_host = obj._host;
	
	return *this;
}

int	Server::setupServer(void)
{
	int	on = 1;

	if (getHost().empty())
		setHost("0.0.0.0");
	if (getPort() <= 0)
		setPort(8080);
	setAddr(AF_INET, htonl(to_uint32_t(getHost())), htons(getPort()));
	setSock(socket(AF_INET, SOCK_STREAM, 0));
	
	if (getSock() < 0)
	{
		perror("Error: socket failed");
		return 1;
	}
	fcntl(getSock(), F_SETFL, O_NONBLOCK);
	if (setsockopt(getSock(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		perror("Error: setsockopt  failed");
		return 1;
	}
	if (bind(getSock(), (sockaddr *)&getAddr(), sizeof(getAddr())) < 0)
	{
		perror("Error: bind failed");
		return 1;
	}
	if (listen(getSock(), 100) < 0)
	{
		perror("Error: listen failed");
		return 1;
	}
	else
		std::cout << _server_name  << " [" << _port << "] Listening..." << std::endl;
	return 0;
}


//GETTERS

int	Server::getSock(void)
{
	return _socket;
}

sockaddr_in	&Server::getAddr(void)
{
	return _addr;
}

int	Server::getPort(void)
{
	return _port;
}

std::string	Server::getServerName(void)
{
	return _server_name;
}

std::string	Server::getRoot(void)
{
	return _root;
}

std::map<std::string, std::string>	Server::getErrorPages(void)
{
	return _error_pages;
}

std::string	Server::getIndex(void)
{
	return _index;
}

size_t	Server::getMaxBody(void)
{
	return _client_max_body_size;
}

std::string	Server::getHost(void)
{
	return _host;
}

//SETTERS

void	Server::setSock(int sock)
{
	_socket = sock;
}

void	Server::setAddr(sa_family_t sin_family, in_addr_t s_addr, in_port_t sin_port)
{
	_addr.sin_family = sin_family;
	_addr.sin_addr.s_addr = s_addr;
	_addr.sin_port = sin_port;
}

void	Server::setPort(int port)
{
	_port = port;
	if (port == 0)
		_port = 8080;
}

void	Server::setServerName(std::string name, char **env)
{
	_server_name = name;
	if (name.empty())
		_server_name = "defaul";
	if (!name.empty() && name[0] == '$')
		_server_name = returnEnv(env, name);
	//std::cout << "Server Name --> " << _server_name << std::endl;
}

void	Server::setRoot(std::string root, char **env)
{
	_root = root;
	if (!root.empty() && root[0] == '$')
		_root = returnEnv(env, root);
	if (root.empty())
		_root = "./html";//default root
	if (_root.find_last_of('/') == _root.size() - 1)
		_root.erase(_root.size() - 1, _root.size());
}

void	Server::setErrorPages(std::string error_pages)
{
	if (error_pages.empty())
    return;

	std::stringstream strs(error_pages);
	std::string num;
	int			error;
	std::string page;

	while (strs >> num >> page)
	{
		bool isAlphanumeric = ft_stoi(num);
		bool endsWithHtml = (page.size() >= 5 && page.substr(page.size() - 5) == ".html");

		if (isAlphanumeric && endsWithHtml)
			_error_pages[num] = page;
		else
			std::cerr << "Invalid input pair: " << num << " " << page << std::endl;
	}
	//separar el error de la pagina de error
}

void	Server::setIndex(std::string index)
{
	_index = index;
	if (index.empty())
		_index = "index.html";//
}

void	Server::setMaxBody(size_t body_size)
{
	_client_max_body_size = body_size;
	if (body_size == 0)
		_client_max_body_size = 1024;//
	if (body_size > 2147483647)
		_client_max_body_size  = 2147483647;
}

void	Server::setHost(std::string host)
{
	_host = host;
	if (host.empty())
		_host = "0.0.0.0";
}
