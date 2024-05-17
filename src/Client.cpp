#include "../inc/Client.hpp"

Client::Client(void)
{}

Client::Client(size_t n): _server_n(n)
{
	_reqSize = 0;
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

	return *this;
}

void	Client::setLastReq(char *req)
{
	if (req)
		_last_req = req;
}
