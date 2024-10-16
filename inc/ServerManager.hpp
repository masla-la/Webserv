#ifndef _SERVERMANAGER_HPP_
# define _SERVERMANAGER_HPP_

# include "webserv.hpp"

# include "Server.hpp"
# include "Client.hpp"
# include "Request.hpp"

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

		char **							_env;

	public:

		ServerManager( void );
		ServerManager( const ServerManager & obj );
		~ServerManager( void );
		ServerManager	&	operator=( const ServerManager & obj );

		int		InitServer( std::vector<Server> confServ );

		void	addToSet( int fd, fd_set *set );
		void	removeFromSet( int fd, fd_set *set );
		void	selectFd( fd_set *read, fd_set *write );
		void	waitClient( void );
		void	acceptClient( void );

		void		handle_request( void );
		bool		client_request( Client & client );
		std::string	readHttpRequest( int socket, Server &  server );

		bool	checkRequest( Client & client );
		bool	checkMethod( std::string method, std::vector<std::string> methods_list );
		bool	checkIndex( std::string path, std::string index );
		
		void	metodGet( Client & client, std::string url, Location *location );
		void	metodPost( Client & client, std::string url, Request & request );
		void	metodDelete( Client & client, std::string url );

		void	listing( Client & client, std::string url, std::string path );
		void	redir( Client & client, std::string redir );

	//Utils
		void		handle_request_error( int error_code, Client & client, fd_set & fd_set, int i );
		void		sendError( int error, Client & client );
		void		sendChuncked( std::string, Client & client, int error );
		void		sendPage( std::string page, Client & client, int code );
		bool		writePost( std::string path, Client & client, std::string str );
		std::string	findType( std::string page );

	//SETTERS
		void	setServer();
		void	setClient();
		void	setErrors();
		void	setEnv( char **env );

	//GETTERS
		std::vector<Server>		getServer( void ){ return _server; };
		std::vector<Client>		getClient( void ){ return _client; };
		std::string				getError( int n );
		char **					getEnv( void );

};

#endif