#ifndef _SERVERMANAGER_HPP_
# define _SERVERMANAGER_HPP_

# include "webserv.hpp"

# include "Server.hpp"
# include "Client.hpp"
# include "Request.hpp"
//# include "Response.hpp"

class ServerManager
{

	private:

		std::vector<Client>				_client;
		std::vector<Server>				_server;
		fd_set							_read_set;
		fd_set							_write_set;
		int								_last_fd;

		std::map<int, std::string>		_errors;

	public:

		ServerManager( void );
		ServerManager( const ServerManager & obj );
		~ServerManager( void );
		ServerManager	&	operator=( const ServerManager & obj );

		//---
		//int		setupServer( std::string port, std::string host, int n );
		int		InitServer( std::vector<Server> const & confServ );
		//---
		void	addToSet( int fd, fd_set *set );
		void	removeFromSet( int fd, fd_set *set );
		void	selectFd( fd_set *read, fd_set *write );
		void	waitClient( void );
		void	acceptClient( void );
		void	handle_request( void );

		void	metodGet( Client & client, std::string url );
		void	metodPost( Client & client, std::string url, Request & request );
		void	metodDelete();

		//Utils
		void		sendError( int error, Client & client );
		void		sendPage( std::string page, Client & client, int code );
		bool		writePost(std::string path, Client client, std::string str);

		std::string	findType( std::string page );
		//---

	//SETTERS
		void	setServer();
		void	setClient();
		void	setErrors();

	//GETTERS
		Server		getServer( unsigned int n ){ return _server[n]; };//
		Client		getClient( unsigned int n ){ return _client[n]; };//
		std::string	getError( int n );
};

#endif