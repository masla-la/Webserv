#include "../inc/ServerManager.hpp"

ServerManager::ServerManager(void): _last_fd(-1)
{
	setErrors();
	_default_error = "HTTP/1.1 404 Not Found\n";
	_default_error += "Content-Type: text/plain\n";
	_default_error += "Content-Length: 15\n\n";
	_default_error += "404 Not Found\n";
	
	_timeout.tv_sec	= 30;

	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);
}

ServerManager::ServerManager(const ServerManager &obj)
{
	*this = obj;
}

ServerManager::~ServerManager(void)
{
	FD_ZERO(&_write_set);
	FD_ZERO(&_read_set);
	for (size_t i = 0; i < _client.size(); i++)
		close(_client[i].getSock());
	for (size_t i = 0; i < _server.size(); i++)
		close(_server[i].getSock());
}

ServerManager	&ServerManager::operator=(const ServerManager &obj)
{
	_client = obj._client;
	_server = obj._server;
	_read_set = obj._read_set;
	_write_set = obj._write_set;
	_last_fd = obj._last_fd;

	_errors = obj._errors;
	_default_error = obj._default_error;

	_timeout = obj._timeout;

	return *this;
}

int	ServerManager::InitServer(std::vector<Server> confServ)
{
	_server = confServ;
	for (size_t i = 0; i < _server.size(); i++)
		if (_server[i].setupServer())
			_server.erase(_server.begin() + i);
	return 0;
}

void	ServerManager::addToSet(int fd, fd_set *set)
{
	FD_SET(fd, set);
	if (fd > _last_fd)
		_last_fd = fd;
}

void	ServerManager::removeFromSet(int fd, fd_set *set)
{
	FD_CLR(fd, set);
	if (fd == _last_fd)
		_last_fd--;
}

void	ServerManager::selectFd(fd_set *read, fd_set *write)
{
	int	i = 0;

	if ((i = select(_last_fd + 1, read, write, 0, &_timeout)) < 0)
		std::cout << "Error: select failed" << std::endl;
	else if (i == 0)
	{
		time_t current_time = time(NULL);
		for (size_t n = 0; n < _client.size(); n++)
		{
			if (current_time - _client[n].getTime() == 30)
			{
				std::cout << "Select time out" << std::endl;//
				removeFromSet(_client[n].getSock(), read);
				close(_client[n].getSock());//
				_client.erase(_client.begin() + n);
			}
		}
	}
	_read_set = *read;
	_write_set = *write;
}

void	ServerManager::waitClient()
{
	fd_set	read;
	fd_set	write;

	FD_ZERO(&read);
	FD_ZERO(&write);
	for(size_t i = 0; i < _server.size(); i++)
		addToSet(_server[i].getSock(), &read);
	for(size_t i = 0; i < _client.size(); i++)
		addToSet(_client[i].getSock(), &read);
	selectFd(&read, &write);
}

void	ServerManager::acceptClient()
{
	sockaddr_in addr;
	socklen_t	size = sizeof(addr);

	for(size_t i = 0; i < _server.size(); i++)
	{
		if (FD_ISSET(_server[i].getSock(), &_read_set))
		{
			Client	client(i);
			client.setSock(accept(_server[i].getSock(), (sockaddr *)&addr, &size));
			_client.push_back(client);
			if (client.getSock() < 0)
			{
				perror("Error: Accept failed");
				exit(1);
			}
			std::cout << "New connection" << std::endl;
		}
	}
}

void	ServerManager::handle_request()
{
	for (size_t i = 0; i < _client.size(); i++)
	{
		if (FD_ISSET(_client[i].getSock(), &_read_set))
		{
			_client[i].setTime(time(NULL));
			char	req[MAX_REQUEST_SIZE + 1];//max request size != 1024
			size_t	reqSize = recv(_client[i].getSock(), req, MAX_REQUEST_SIZE, 0);

			_client[i].setReqSize(reqSize);//
			_client[i].setLastReq(req);

			if (reqSize < 0)
			{
				std::cout << "Error: Recv failed" << std::endl;
				sendError(500, _client[i]);//
				removeFromSet(_client[i].getSock(), &_read_set);
				close(_client[i].getSock());
				_client.erase(_client.begin() + i);
				i--;
			}
			else if (reqSize == 0)
			{
				std::cout << "Connection is closed" << std::endl;
				removeFromSet(_client[i].getSock(), &_read_set);
				close(_client[i].getSock());
				_client.erase(_client.begin() + i);
				i--;
			}
			else//check request valid falta
			{
				std::cout << "New Request" << std::endl;
				Request	request(_client[i].getLastReq().c_str());;

				int	ret = -1;
				if ((ret = request.checkProt()) != -1)
				{
					sendError(ret, _client[i]);
					removeFromSet(_client[i].getSock(), &_read_set);
					close(_client[i].getSock());
					_client.erase(_client.begin() + i);
					i--;
					continue;
				}

				std::string	url = request.getUrl();
				std::string query;
				size_t		pos;
				if ((pos = url.rfind("?")) != std::string::npos)//
				{
					query = url.substr(pos, url.size());
					url = url.substr(0, pos);
				}
				if (request.getLen() != std::string::npos && request.getLen() > _server[_client[i].getServ()].getMaxBody())
				{
					sendError(413, _client[i]);
					removeFromSet(_client[i].getSock(), &_read_set);
					close(_client[i].getSock());
					_client.erase(_client.begin() + i);
					i--;
					continue;
				}

				
				//location
				//cgi
				Location	*location = _server[_client[i].getServ()].getLocation(url);

				if (checkMethod(request.getMethod(), _server[_client[i].getServ()].getMethods()) && \
				(location != NULL && checkMethod(request.getMethod(), location->getMethods())))
				{
					sendError(405, _client[i]);
					removeFromSet(_client[i].getSock(), &_read_set);
					close(_client[i].getSock());
					_client.erase(_client.begin() + i);
					i--;
					continue;
				}
				else if (is_cgi(url))
				{
					//---
					std::cout << "CGI" << std::endl;
					//---
					cgi_ex(url, query, _client[i], _server[_client[i].getServ()]);
				}
				else
				{
					if (location && !location->getRedir().empty())
						redir(_client[i], location->getRedir());
					else if (request.getMethod() == "GET")
						metodGet(_client[i], url, location);
					else if (request.getMethod() == "POST")
						metodPost(_client[i], url, request);
					else if (request.getMethod() == "DELETE")
						metodDelete(_client[i], url);
				}
				if (_client[i].getSock() <= 0)
				{
					removeFromSet(_client[i].getSock(), &_read_set);
					_client.erase(_client.begin() + i);
				}
				/////
				//http /1.1 mantiene  la conexion abierta a menos  que la request lo indique -> Connection: close
				if (_client[i].getSock())
				{
					removeFromSet(_client[i].getSock(), &_read_set);
					close(_client[i].getSock());
					_client.erase(_client.begin() + i);
					i--;
				}
			}
		}
	}
}	

void	ServerManager::sendError(int error, Client & client)
{
	std::map<std::string, std::string> errorPages;
	int fd;

	errorPages = _server[client.getServ()].getErrorPages();
	if (!errorPages[ft_size_to_str(error)].empty() && errorPages[ft_size_to_str(error)].size() <= 0)
	{
		fd = open((_server[client.getServ()].getRoot() + '/' + errorPages[ft_size_to_str(error)]).c_str(), O_RDONLY);
		if (fd < 0)
		{
			std::cout << "Error: Error pages failed -> " << error << std::endl;
			errorPages.erase(errorPages.find(ft_size_to_str(error)));
			sendError(error, client);
			return;
		}
		close(fd);
		sendPage(_server[client.getServ()].getRoot() + '/' + errorPages[ft_size_to_str(error)], client, 200);
	}
	else
	{
		if (!_errors[error].empty())
		{
			//---
			std::cout << "Error Page: " << _errors[error] << std::endl;
			//---
			int i;
			std::string msg = "HTTP/1.1 ";
			msg += _errors[error] + "\n";
			msg += "Content-Type: text/plain\n";
			msg += "Content-Length: ";
			msg += ft_size_to_str(_errors[error].length() + 1);
			msg += "\n\n";
			msg += _errors[error] + "\n";
			i = send(client.getSock(), msg.c_str(), msg.length(), 0); //
			if (i < 0)
				std::cout << "Client disconnected" << std::endl;
			else if (i == 0)
				std::cout << "0 byte passed to server" << std::endl;
		}
	}
}

void	ServerManager::sendPage(std::string page, Client & client, int error)
{
	std::cout << "Show Page: " << page << std::endl;

	if (page.empty())
	{
		std::string msg = "HTTP/1.1 ";
		msg += _errors[error];
		msg += "\n\n";
		size_t i;
		if ((i = send(client.getSock(), msg.c_str(), msg.size(), 0)) <= 0)
			sendError(500, client);
		return ;
	}
	else
	{
		// Sección para enviar el contenido de la página
		std::ifstream fd(page.c_str());
		if (!fd.is_open())
		{
			sendError(404, client);
			return;
		}

		// Obtener el tamaño del archivo
		fd.seekg(0, std::ios::end);
		size_t size = fd.tellg();
		fd.seekg(0, std::ios::beg);

		std::string type = findType(page);

		// Construir los encabezados de la respuesta HTTP
		std::string msg = "HTTP/1.1 ";
		msg += _errors[error];
		msg += "\nContent-Type: ";
		msg += type;
		msg += "\nContent-Length: ";
		msg += ft_size_to_str(size);//
		msg += "\n\n";

		// Enviar los encabezados
		size_t i;
		if ((i = send(client.getSock(), msg.c_str(), msg.size(), 0)) <= 0) {
			sendError(500, client);
			return;
		}

		// Enviar el contenido del archivo
		char buffer[1024];
		while (size > 0)
		{
			int bytes_to_read = std::min(static_cast<size_t>(1024), size);
			fd.read(buffer, bytes_to_read);
			if (fd.eof() && fd.fail())
			{
				sendError(500, client);
				return;
			}
			size_t n;
			if ((n = send(client.getSock(), buffer, fd.gcount(), 0)) <= 0)
			{
				sendError(500, client);
				return;
			}
			size -= n;
		}
		fd.close();
	}
}

bool	ServerManager::writePost(std::string path, Client &client, std::string str)
{
	int	fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd < 0)
	{
		sendError(500, client);
		return false;
	}

	addToSet(fd, &_write_set);
	selectFd(&_read_set, &_write_set);

	if (write(fd, str.c_str(), str.length()))
	{
		sendError(500, client);
		close(fd);
		return false;
	}
	close(fd);
	return true;

}

std::string	ServerManager::findType(std::string page)
{
	std::string	tmp = page;
	size_t i = page.find_last_of(".") + 1;
	tmp.erase(0, i);
	
	if (tmp == "png")
		return "image/png";
	if (tmp == "jpg")
		return "image/jpeg";
	if (tmp == "gif")
		return "image/gif";
	if (tmp == "html")
		return "text/html";
	if (tmp == "css")
		return "text/css";
	if (tmp == "pdf")
		return "application/pdf";
	if (tmp == "json")
		return "application/json";
	if (tmp == "mp4")
		return "video/mp4";
	return "text/plain";
}

//METHODS

bool	ServerManager::checkMethod(std::string method, std::vector<std::string> methods_list)
{
	for (size_t i = 0; i < methods_list.size(); i++)
		if (methods_list[i] == method)
			return true;
	return false;
}

bool	ServerManager::checkIndex(std::string path, std::string index)
{
	std::string indexFile = path + '/' + index;
	struct stat stat_index;
	if (stat(indexFile.c_str(), &stat_index) == 0 && S_ISREG(stat_index.st_mode))
		return true;
	return false;
}

void	ServerManager::metodGet(Client &client, std::string url, Location *location)
{
	//---
	std::cout << "Get Method\n";
	//---
	if (url.size() >= 64)
	{
		sendError(414, client);
		return ;
	}

	std::string	path = _server[client.getServ()].getRoot() + url;

	if (location && !location->getIndex().empty() && checkIndex(path, location->getIndex()))
	{
		sendPage(path + '/' + location->getIndex(), client, 200);
		return ;
	}

	struct stat	stat_path;
	int	fd = open(path.c_str(), O_RDONLY);
	stat(path.c_str(), &stat_path);

	if (fd <= 0)
	{
		sendError(404, client);
		return ;
	}
	if (S_ISDIR(stat_path.st_mode))
	{		
		if (checkIndex(path, _server[client.getServ()].getIndex()) && !location)
			sendPage(path + '/' + _server[client.getServ()].getIndex(), client, 200);
		else if (_server[client.getServ()].getListing() && location && location->getListing())
			listing(client, url, path);
		else
			sendError(404, client);
	}
	else
		sendPage(path, client, 200);
	close(fd);
}

void	ServerManager::metodPost(Client &client, std::string url, Request &request)
{
	//---
	std::cout << "Post Method\n";
	//---
	if (request.getHeader()["Transfer-Encoding"] == "chunked")//
	{
		sendError(411, client);
		return  ;
	}
	std::string	path = _server[client.getServ()].getRoot() + url;
	struct stat	stat_path;
	lstat(path.c_str(), &stat_path);

	if (S_ISDIR(stat_path.st_mode))
	{
		std::string	body = request.getFullBody();
		std::string	file;
	
		if (!(request.getHeader().empty() && request.getBoundary().empty()))
		{
			std::cout << "Post in directory: " << std::endl;
			size_t start = 0;
			while (true)
			{
				start = body.find("name=\"", start);
				if (start == std::string::npos)
					break;
				start += 6;
				size_t end = body.find("\"", start);
				if (end == std::string::npos)
					break;
				std::string name = body.substr(start, end - start);
				std::cout << "+ " + name << std::endl;

				start = body.find("\r\n\r\n", end);
				if (start == std::string::npos)
					break;
				start += 4;
				end = body.find(request.getBoundary(), start);
				if (end == std::string::npos)
					break;

				file = body.substr(start, end - start - 4);

				if (!writePost(url + "/" + name, client, file))
					break;

				if (body[end + request.getBoundary().size()] == '-')
					break;
			}
		}
		else
		{
			sendError(400, client);//
			return ;
		}
	}
	else
	{
		std::cout << "POST IN FILE\n";//
		if (!writePost(path, client, request.getFullBody()))
			return ;
	}
	if (request.getLen() == 0)
		sendPage("", client, 204);
	else
		sendPage("", client, 201);
}

void	ServerManager::metodDelete(Client &client, std::string url)
{
	//---
	std::cout << "Delete Method\n";
	//---
	std::string	path = _server[client.getServ()].getRoot() + url;
	std::ifstream	fd(path.c_str());
	if (!fd)
	{
		sendError(404, client);
		return ;
	}
	fd.close();
	std::remove(path.c_str());

	if (send(client.getSock(), "HTTP /1.1 200 OK\n", 18, 0) <= 0)
		sendError(500, client);
}

void	ServerManager::listing(Client &client, std::string url, std::string path)
{
	//---
	std::cout << "Directiry Listing\n";
	//---
	DIR				*dir;
	struct dirent	*ent;
	std::string		data;

	std::string	msg = "HTTP/1.1 200 OK\n";
	msg += "Content-Type: text/html\n\n";
	msg += "<!DOCTYPE html>\n<html>\n<body>\n<h1>" + url + "</h1>\n<pre>\n";

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string	name = ent->d_name;
			if (name == "." || name == "..")
				continue ;
			std::string href = url + "/" + name;
			msg += "<a href=\"" + href + "\">" + name + "</a>\n";
		}
		closedir(dir);
	}
	else
	{
		std::cout << "Error: directory listing" << std::endl;
		sendError(500, client);
		return ;
	}

	msg += "</pre>\n</body>\n</html>\n";

	int i;
	if ((i = send(client.getSock(), msg.c_str(), msg.length(), 0)) < 0)
		sendError(500, client);
	else if (i == 0)
		sendError(400, client);
}

void	ServerManager::redir( Client & client, std::string redir )
{
	//---
	std::cout << "Redirect to: " << redir << std::endl;
	//---
	std::string	msg = "HTTP/1.1 200 OK\n\n";
	msg += "<head><meta http-equiv=\"refresh\" content = \"0;url=";
	msg += redir;
	msg += "\" /></head>";

	int i;

	if ((i = send(client.getSock(), msg.c_str(), msg.length(), 0)) < 0)
		sendError(500, client);
	else if (i == 0)
		sendError(400, client);
}


//SETTERS
void	ServerManager::setErrors()
{
	_errors[200] = "200 OK";
	_errors[201] = "201 Created";
	_errors[204] = "204 No Content";
	_errors[300] = "300 Multiple Choices";
	_errors[301] = "301 Moved Permanently";
	_errors[302] = "302 Found";
	_errors[303] = "303 See Other";
	_errors[307] = "307 Temporary Redirect";
	_errors[400] = "400 Bad Request";
	_errors[404] = "404 Not Found";
	_errors[405] = "405 Method Not Allowed";
	_errors[408] = "408 Request Time Out";
	_errors[411] = "411 Length Required";
	_errors[413] = "413 Request Entity Too Large";
	_errors[414] = "414 Request-URI Too Long";
//---
	_errors[418] = "418 I'm a teapot";
//---
	_errors[500] = "500 Internal Server Error";
	_errors[502] = "502 Bad Gateway";
	_errors[505] = "505 HTTP Version Not Supported";
}

//GETTERS
std::string	ServerManager::getError(int n)
{
	std::map<int, std::string>::iterator it;
	it = _errors.find(n);
	if (it != _errors.end())
		return it->second;
	return "";
}