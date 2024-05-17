#include "../inc/webserv.hpp"

std::string	ft_size_to_str(size_t n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string	ft_size_to_str(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}
