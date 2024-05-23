#ifndef _SERVERMANAGER_HPP_
# define _SERVERMANAGER_HPP_

# include "webserv.hpp"

# include "Server.hpp"
# include "Client.hpp"
# include "Request.hpp"
//# include "Response.hpp"

class	ServerManager
{

	private:

		std::vector<Client>				_client;
		std::vector<Server>				_server;
		fd_set							_read_set;
		fd_set							_write_set;
		int								_last_fd;

		std::map<int, std::string>		_errors;
		std::string						_default_error;

		struct timeval					_timeout;

	public:

		ServerManager( void );
		ServerManager( const ServerManager & obj );
		~ServerManager( void );
		ServerManager	&	operator=( const ServerManager & obj );

		//---
		//int		setupServer( std::string port, std::string host, int n );
		int		InitServer( std::vector<Server> confServ );
		//---
		void	addToSet( int fd, fd_set *set );
		void	removeFromSet( int fd, fd_set *set );
		void	selectFd( fd_set *read, fd_set *write );
		void	waitClient( void );
		void	acceptClient( void );
		void	handle_request( void );

		bool	checkMethod( std::string method, std::vector<std::string> methods_list );
		bool	checkIndex(std::string path, std::string index);
		void	metodGet( Client & client, std::string url, Location *location );
		void	metodPost( Client & client, std::string url, Request & request );
		void	metodDelete( Client & client, std::string url );
		void	listing( Client & client, std::string url, std::string path, int sock );

		//Utils
		void		sendError( int error, Client & client );
		void		sendPage( std::string page, Client & client, int code );
		bool		writePost(std::string path, Client & client, std::string str);
		std::string	findType( std::string page );
		//---

	//SETTERS
		void	setServer();
		void	setClient();
		void	setErrors();

	//GETTERS
		std::vector<Server>		getServer( void ){ return _server; };//
		std::vector<Client>		getClient( void ){ return _client; };//
		std::string				getError( int n );
};

#endif