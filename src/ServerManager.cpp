#include "../inc/ServerManager.hpp"

ServerManager::ServerManager(void)
{
	//Server	server;

	//_server.push_back(server);
	setErrors();
}

ServerManager::ServerManager(const ServerManager &obj){}

ServerManager::~ServerManager(void)
{
	for(unsigned int i = 0; i < _server.size(); i++)
		close(_server[i].getSock());//
}

ServerManager	&ServerManager::operator=(const ServerManager &obj)
{
	return *this;//
}

int	ServerManager::InitServer(std::vector<Server> const & confServ)
{
	for (size_t i = 0; i < confServ.size(); i++)
		_server.push_back(confServ[i]);
//comprobar q error_pages funciona y se copia correctamente
	for (size_t i = 0; i < _server.size(); i++)
		_server[i].setupServer();
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
	if ((i = select(_last_fd + 1, read, write, 0, 0)) < 0)
		exit (1);//
	else if (i == 0)
		std::cout << "Select time out" << std::endl;//
	_read_set = *read;
	_write_set = *write;
}

void	ServerManager::waitClient()
{
	fd_set	read;
	fd_set	write;

	FD_ZERO(&read);
	FD_ZERO(&write);
	for(unsigned int i = 0; i < _server.size(); i++)
		addToSet(_server[i].getSock(), &read);
	for(unsigned int i = 0; i < _client.size(); i++)
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
			Client	client;
			client.init(i);//
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
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (FD_ISSET(_client[i].getSock(), &_read_set))
		{
			char	req[MAX_REQUEST_SIZE + 1];//max request size != 1024
			size_t	reqSize = recv(_client[i].getSock(), req, MAX_REQUEST_SIZE, 0);

			_client[i].setReqSize(reqSize);//
			_client[i].setLastReq(req);

			int headerSize = _client[i].getLastReq().find("\r\n\r\n", 0) + 4;

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

				//request.requestInit(_client[i].getLastReq().c_str());
				//metodo valido
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
				if (request.getMethod() == "GET")
					metodGet(_client[i], url);
				else if (request.getMethod() == "POST")
					metodPost(_client[i], url, request);
				else if (request.getMethod() == "DELETE")
					metodDelete();
				
				/////
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

    errorPages = _server[client.getServ()].getErrorPages(); //
    if (errorPages.find(std::to_string(error)) != errorPages.end()) //
    {
        fd = open(errorPages[std::to_string(error)].c_str(), O_RDONLY);
        if (fd < 0)
        {
            std::cout << "Error: Error pages failed" << std::endl;
            // Si falla la apertura del archivo, no es necesario cerrarlo.
            return;
        }
        close(fd); // Se debe cerrar el descriptor de archivo después de su uso.
        sendPage(errorPages[std::to_string(error)], client, 200);
    }
    else
    {
        if (!_errors[error].empty())
        {
            int i;
            std::string msg = "HTTP/1.1 ";
            msg += _errors[error] + "\n";
            msg += "Content-Type: text/plain\n";
            msg += "Content-Length: ";
            msg += std::to_string(_errors[error].length() + 1);
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

void	ServerManager::sendPage(std::string page, Client & client, int code)
{
    std::cout << "Show Page: " << page << std::endl;

    if (page.empty())
	{
        std::string msg = "HTTP/1.1 ";
        msg += _errors[code];
        msg += "\n\n";
        size_t i;
        if ((i = send(client.getSock(), msg.c_str(), msg.size(), 0)) <= 0)
            sendError(500, client);
        return ;
    }
	else
	{
        // Sección para enviar el contenido de la página
        std::ifstream fd(page);
        if (!fd.is_open())
		{
            sendError(500, client);
            return;
        }

        // Obtener el tamaño del archivo
        fd.seekg(0, std::ios::end);
        size_t size = fd.tellg();
        fd.seekg(0, std::ios::beg);

        std::string type = findType(page);

        // Construir los encabezados de la respuesta HTTP
        std::string msg = "HTTP/1.1 ";
        msg += _errors[code];
        msg += "\nContent-Type: ";
        msg += type;
        msg += "\nContent-Length: ";
        msg += std::to_string(size);//
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

bool	ServerManager::writePoll(std::string path, Client client, std::string str)
{

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
void	ServerManager::metodGet(Client &client, std::string url)
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

	//location

	struct stat	stat_path;
	int	fd = open(path.c_str(), O_RDONLY);
	stat(path.c_str(), &stat_path);

	if (!fd)
	{
		sendError(404, client);
		close(fd);
		return ;
	}
	if (S_ISDIR(stat_path.st_mode))
	{
		if (url[0] == '/')
			sendPage(path + _server[client.getServ()].getIndex(), client, 200);
		//else if ()
		//location
		else
			sendError(404, client);
	}
	else
		sendPage(path, client, 200);
	close(fd);
}

void	ServerManager::metodPost(Client &client,  std::string url, Request &request)
{
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
			std::cout << "Post in directory : " << std::endl;
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

				if (!writePoll(url + "/" + name, client, file))
					break;

				if (body[end + request.getBoundary().size()] == '-')
					break;
			}
		}
	}
	else
	{
		std::cout << "POST IN FILE\n";//
	}
	if (request.getLen() == 0)
		sendPage("", client, 204);
	else
		sendPage("", client, 201);
	//---
	std::cout << "Post Method\n";
	//---
}

void	ServerManager::metodDelete()
{
	//---
	std::cout << "Delete Method\n";
	//---
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