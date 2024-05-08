#include "../inc/webserv.hpp"

std::string	ft_size_to_str(size_t n)
{
	std::string dest;

	if (n < 10)
		dest += ft_size_to_str(n / 10);
	dest = (n % 10) + '0';
	return  dest;
}