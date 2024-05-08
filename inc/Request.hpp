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

	public:

		//Request( void );
		Request( const char *req );
		//Request( const Request & obj );//
		~Request( void );
		//Request	&	operator=( const Request & 	obj );

		//void			requestInit( const char *req );

		void		makeQuery( void );
		void		makeGet( std::stringstream & strs );
		void		makePost( std::stringstream & strs );
		int			checkProt( void );

		//
		size_t	getLen(){return 10;};//
		//

	//GETTERS
		std::string		getRequest( void ){ return _request; };
		std::string		getMethod( void ){ return _method; };
		std::string		getUrl( void ){ return _url; };
		std::string		getProtocol( void ){ return _protocol; };
};

#endif