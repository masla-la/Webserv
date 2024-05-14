#include "../inc/Client.hpp"

void	Client::init(size_t n)
{
	_server_n = n;
	_time = 0;//
}

void	Client::setLastReq(char *req)
{
	_last_req = req;
}
