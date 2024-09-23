#ifndef _REQUEST_HPP_
# define _REQUEST_HPP_

# include "webserv.hpp"
# include "Client.hpp"

class	Request
{

	private:

		std::string	_request;
		std::string	_method;
		std::string	_url;
		std::string	_protocol;

		std::string	_query;

		std::map<std::string, std::string>	_header;
		std::string							_boundary;
		std::string							_body;
		std::string							_full_body;
		size_t								_len;

		Request( void );
	
	public:

		Request( const char *req );
		Request( const Request & obj );
		~Request( void );
		Request	&	operator=( const Request & 	obj );

		void		makeQuery( void );
		void		makeGet( std::stringstream & strs );
		void		makePost( std::stringstream & strs );
		int			checkProt( void );

	//GETTERS
		std::string		getRequest( void );
		std::string		getMethod( void );
		std::string		getUrl( void );
		std::string		getProtocol( void );
		size_t			getLen( void );

		std::string		getBoundary( void );
		std::string		getBody( void );
		std::string		getFullBody( void );
		std::map<std::string, std::string>		getHeader( void );

};

#endif