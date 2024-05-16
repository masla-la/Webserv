#ifndef _CONFIG_HPP_
# define _CONFIG_HPP_

# include "../webserv.hpp"
# include "../Server.hpp"
# include "../Location.hpp"

class	Server;
class	Location;

class	Config
{
	private:

		std::vector<std::string>	_fd;
		std::vector<Server>			_server;
		//std::vector<Location>		_location;
		std::vector<std::string>	_directives;

	public:

		Config( void );
		~Config( void );

		void	readFd( std::string namefd );
		void	checkConfig( char **env );
		void	checkDirective( std::string line, std::string conf, Server & server, char **env );
		size_t	parseLocation( size_t n, std::string dir, Server server );

		std::vector<Server>	getServ( void );

	//Exceptions
		class ConfError : public std::exception
			{ public:
				virtual const char *what( void ) const throw()
			{ return "Error: Config failed"; } };
		class EmptyFile : public std::exception
			{ public:
				virtual const char *what( void ) const throw()
			{ return "Error: Empty File"; } };
		class BadConfig : public std::exception
			{ public:
				virtual const char *what( void ) const throw()
			{ return "Error: Unknown directive"; } };

};

#endif