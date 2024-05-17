#ifndef _CLIENT_HPP_
# define _CLIENT_HPP_

#include "webserv.hpp"

class Client
{

	private:

		int				_socket;
		size_t			_server_n;
		std::string		_last_req;
		
		size_t			_reqSize;

		Client( void );

	public:

		Client( size_t n );
		Client( const Client & obj );
		~Client( void );
		Client	&operator=(Client const &obj);


	//GETTERS
		int			getSock( void ){ return _socket; };
		std::string	getLastReq( void ){ return _last_req; };
		int			getServ( void ){ return _server_n; };
	//SETTERS
		void		setSock( int socket ){ _socket = socket; };
		void		setLastReq( char *req );

		void		setReqSize( size_t i){ _reqSize = i; };
};

#endif