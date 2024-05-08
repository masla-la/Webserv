#ifndef _RESPONSE_HPP_
# define _RESPONSE_HPP_

# include "webserv.hpp"

# include "Client.hpp"
# include "Request.hpp"

class	Response
{

	private:

		

	public:

		Response( void );
		Response( const Response & obj );//
		~Response( void );
		Response	&	operator=( const Response & obj );

};

	void			metodGet( Client &client, std::string url );
	void			metodPost( Client &client, std::string url, Request req );
	void			metodDelete( Client &client, std::string url);



#endif