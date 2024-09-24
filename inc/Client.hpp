#ifndef _CLIENT_HPP_
# define _CLIENT_HPP_

#include "webserv.hpp"

class Client
{

	private:

		int				_socket;
		size_t			_server_n;
		std::string		_last_req;
		size_t			_req_size;

		time_t	_time;

		Client( void );

	public:

		Client( size_t n );
		Client( const Client & obj );
		~Client( void );
		Client	&operator=( Client const &obj );

	//GETTERS
		int			getSock( void );
		std::string	getLastReq( void );
		int			getServ( void );
		time_t		getTime( void );
		size_t		getReqSize( void );

	//SETTERS
		void		setSock( int socket );
		void		setLastReq( std::string req );
		void		setReqSize( size_t i );
		void		setTime( time_t time );

};

#endif