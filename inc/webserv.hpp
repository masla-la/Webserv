#ifndef _WEBSERV_HPP_
# define _WEBSERV_HPP_

//Standar CPP
# include <iostream>
# include <sstream>
# include <string>
# include <fstream>

//Containers
# include <map>
# include <vector>

//System C
# include <sys/wait.h>
# include <stdlib.h>

//Signal C
# include <signal.h>

//Fd & Dir Cntl C
# include <fcntl.h>
# include <dirent.h>
# include <sys/stat.h>

//Network C
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>

# include "conf/Config.hpp"
class	Config;
#include "Server.hpp"
class	Server;
#include "Client.hpp"
class	Client;

# define MAX_REQUEST_SIZE 1024

//Utills
size_t		hex_to_dec( const std::string & hex );
uint32_t	to_uint32_t( const std::string & ip_address );
size_t		ft_stoi( std::string str );
std::string	ft_size_to_str( size_t n );
std::string	ft_size_to_str( int n );

//Parser
void		parser( int ac, char **av, Config &conf, char **env );
bool		isdirective ( std::string line, std::vector<std::string> dir );
std::string	returnEnv( char **env, std::string line );

//CGI
bool		is_cgi(std::string url);
std::string	cgi_ex(std::string url, std::string query, Server & server, char **env);

//Exceptions
class ArgError : public std::exception
	{ public:
		virtual const char *what( void ) const throw()
	{ return "Error: Usage -> ./webserv {CONFIG_FILE}"; } };

class MethodError : public std::exception
	{ public:
		virtual const char *what( void ) const throw()
	{ return "Error: Unknown method"; } };

class ListingError : public std::exception
	{ public:
		virtual const char *what( void ) const throw()
	{ return "Error: Unknown dir_listing"; } };

class StringToIntError : public std::exception
	{ public:
		virtual const char *what( void ) const throw()
	{ return "Number error{ ft_stoi }"; } };

#endif