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

void	Request::makePost(std::stringstream & strs)
{}

int		Request::checkProt()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return 405;
	if (_protocol != "HTTP/1.1")
		return 505;
	return -1;
}

