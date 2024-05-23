#include "../inc/Request.hpp"

Request::Request(void)
{}

Request::~Request(void)
{}

Request::Request(const char *req)
{
	std::stringstream	strs(req);
	_request = req;
	strs >> _method;
	strs >> _url;
	strs >> _protocol;
	_len = 0;
	makeQuery();
	if (_method == "GET")
		makeGet(strs);
	else if (_method == "POST")
		makePost(strs);
}

Request::Request(const Request &obj)
{
	*this = obj;
}

Request	&Request::operator=(const Request &obj)
{
	_request = obj._request;
	_method = obj._method;
	_url = obj._url;
	_protocol = obj._protocol;

	_query = obj._query;

	_header = obj._header;
	_boundary = obj._boundary;
	_body = obj._body;
	_full_body = obj._full_body;
	_len = obj._len;

	return *this;
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
		if (token[token.size() - 1] == ':')
		{
			if (!key.empty() && !line.empty() && key != token)
			{
				if (line[line.size() - 1] == ' ')
					line.resize(line.size() - 2);
				_header[key] = token;
			}
			token.resize(token.size() - 2);
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
		if (line[line.size() - 1] == ' ')
			line.resize(line.size() - 2);
		_header[key] = token;
	}

}

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
				if (line[line.size() - 1] == ' ')
					line.resize(line.size() - 2);
				_header[key] = line;
				line.clear();
			}
			key = token;
			strs >> token;
			_header[key] = token;
			_len = ft_stoi(token);
			key.clear();
		}
		else if (_request.find(token) >= pos - token.length()) 
		{
			pos += 4;
			if (!key.empty() && key != token)
			{
				if (!line.empty())
					line.resize(line.size() - 2);
				if (!line.empty())
					_header[key] = token;
				else
					_header[key] = token;
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
		else if (token[token.size() - 1] == ':')
		{
			if (!key.empty() && key != token)
			{
				if (line[line.size() - 1] == ' ')
					line.resize(line.size() - 2);
				_header[key] = line;//_header.insert(std::make_pair(key, line));
			}
			token.resize(token.size() - 2);
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

	std::cout << "Headers:" << std::endl;
	//for (const auto& pair : _header)
	//	std::cout << pair.first << ": " << pair.second << std::endl;
}

int		Request::checkProt()
{
	if (_method.size() == 0 || _url.size() == 0 || _protocol.size() == 0)
		return 400;
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
