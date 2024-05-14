#include "../inc/Request.hpp"

//Request::Request(void)
//{}

Request::~Request(void)
{
}

Request::Request(const char *req)
{
	std::stringstream	strs(req);
	_request = req;
	strs >> _method;
	strs >> _url;
	strs >> _protocol;
	_len = 0;
	makeQuery();
	//std::cout << _query << "query" << std::endl;
	if (_method == "GET")
		makeGet(strs);
	else if (_method == "POST")
		makePost(strs);
	//
}

void	Request::makeQuery()
{
	size_t pos = _url.find("?");
	if (pos != std::string::npos)
		_query = _url.substr(pos + 1);
}

void	Request::makeGet(std::stringstream & strs)
{
	std::string	token;
	std::string	line;
	std::string	key;

	while (strs >> token)
	{
		if (token.back() == ':')
		{
			if (!key.empty() && !line.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back();
				//_header.inser()
			}
			token.pop_back();
			key = token;
			line.clear();
		}
		else
		{
			if (!line.empty())
				line += " ";
			line += token;
		}
	}
	if (!line.empty() && !key.empty())
	{
		if (line.back() == ' ')
			line.pop_back();
		//_header.insert
	}

}

/*void	Request::makePost(std::stringstream & strs)
{
	std::string	body;
	while (strs >> body)
	{
		std::cout << body << std::endl;//
		if (body.find("boundary=") != std::string::npos);
			_boundary =  body
	}
}*/

void	Request::makePost(std::stringstream &strs)
{
	std::string token;
	std::string	line;
	std::string	key;
	std::string buff;
	size_t pos = _request.find("\r\n\r\n");

	while (strs >> token)
	{
		if (token.find("boundary=") != std::string::npos)
			_boundary = token.substr(token.find("boundary=") + 9);
		else if (token == "Content-Length:")
		{
			if (!key.empty() && !line.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back(); // remove space
				_header[key] = line; //.insert(std::make_pair(key, line));
				line.clear();
			}
			key = token;
			strs >> token;
			_header[key] = token; //.insert(std::make_pair(key, token));
			_len = ft_stoi(token); // Use std::stoi for safe string to int conversion
			key.clear();
		}
		else if (_request.find(token) >= pos - token.length()) 
		{
			pos += 4;
			if (!key.empty() && key != token)
			{
				if (!line.empty())
					line.pop_back(); // remove space
				if (!line.empty())
					_header.insert(std::make_pair(key, line));
				else
					_header.insert(std::make_pair(key, token));
			}
			size_t pos_header = pos;
			while (pos < _len + pos_header && pos < _request.size())
			{
				_full_body += _request[pos];
				pos++;
			}
			if (_boundary.empty())
				_body = _full_body;
			break;
		}
		else if (token.back() == ':')
		{
			if (!key.empty() && key != token)
			{
				if (line.back() == ' ')
					line.pop_back();
				_header.insert(std::make_pair(key, line));
			}
			token.pop_back();
			key = token;
			line.clear();
		}
		else
		{
			if (!line.empty())
				line += " ";
			line += token;
		}
	}

	// Print the headers
	std::cout << "Headers:" << std::endl;
	for (const auto& pair : _header)
		std::cout << pair.first << ": " << pair.second << std::endl;
}

int		Request::checkProt()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return 405;
	if (_protocol != "HTTP/1.1")
		return 505;
	return -1;
}


//GETTERS
std::string	Request::getRequest(void)
{
	return _request;
}

std::string	Request::getMethod(void)
{
	return _method;
}

std::string	Request::getUrl(void)
{
	return _url;
}

std::string	Request::getProtocol(void)
{
	return _protocol;
}

size_t	Request::getLen(void)
{
	return _len;
}

std::string	Request::getBoundary(void)
{
	return _boundary;
}

std::string	Request::getBody(void)
{
	return _body;
}

std::string	Request::getFullBody(void)
{
	return _full_body;
}

std::map<std::string, std::string>	Request::getHeader(void)
{
	return _header;
}
