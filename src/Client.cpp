#include "../inc/Client.hpp"

Client::Client(void)
{}

Client::Client(size_t n): _server_n(n)
{
	_reqSize = 0;
	_time = time(NULL);
}

Client::Client(Client const &obj)
{
	*this = obj;
}

Client::~Client(void)
{}

Client	&Client::operator=(Client const &obj)
{
	_socket = obj._socket;
	_server_n = obj._server_n;
	_last_req = obj._last_req;
	_reqSize = obj._reqSize;

	_time = obj._time;

	return *this;
}

//GETTERS
int			Client::getSock(void)
{
	return _socket;
}

std::string	Client::getLastReq(void)
{
	return _last_req;
}

int			Client::getServ(void)
{
	return _server_n;
}

time_t		Client::getTime(void)
{
	return _time;
}

//SETTERS
void		Client::setSock(int socket)
{
	_socket = socket;
}

void	Client::setLastReq(char *req)
{
	if (req)
		_last_req = req;
}

void		Client::setReqSize(size_t i)
{
	_reqSize = i;
}

void		Client::setTime(time_t time)
{
	_time = time;
}
