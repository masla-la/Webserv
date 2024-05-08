#ifndef _SERVER_HPP_
# define _SERVER_HPP_

# include "webserv.hpp"

class	Server
{

	private:

		int					_socket;
		sockaddr_in			_addr;
		int					_port;//

		std::string									_server_name;
		std::string									_root;
		std::map<std::string, std::string>			_error_pages;
		std::string									_index;
		size_t										_client_max_body_size;
		std::string									_host;

	public:

		Server( void ){};
		Server( const Server & obj ){*this = obj;};//
		~Server( void ){};
		Server	&	operator=( const Server & obj );

		//---
		int		setupServer( void );
		//---

	//GETTERS
		int				getSock( void );
		sockaddr_in		&getAddr( void );
		int				getPort( void );

		std::string								getServerName( void );
		std::string								getRoot( void );
		std::map<std::string, std::string>		getErrorPages( void );
		std::string								getIndex( void );
		size_t									getMaxBody( void );
		std::string								getHost( void );

	//SETTERS
		void		setSock( int sock );
		void		setAddr( sa_family_t sin_family, in_addr_t s_addr, in_port_t sin_port);
		void		setPort( int port );

		void		setServerName( std::string name, char **env );
		void		setRoot( std::string root, char **env );
		void		setErrorPages( std::string error_pages );
		void		setIndex( std::string index );
		void		setMaxBody( size_t body_size);
		void		setHost( std::string host );

};

#endif