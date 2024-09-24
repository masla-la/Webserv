#include "../inc/webserv.hpp"

size_t	hex_to_dec(const std::string & hex)
{
	size_t dec;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> dec;
    return dec;
}

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
