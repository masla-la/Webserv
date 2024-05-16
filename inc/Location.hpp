#ifndef	_LOCATION_HPP_
# define _LOCATION_HPP_

#include "webserv.hpp"

class	Location
{
	private:

		std::string					_dir;
		std::string					_root;
		std::string					_index;
		std::vector<std::string>	_directives;
		std::string					_redir;
		bool						_listing;
		std::vector<std::string>	_methods;

	public:

		Location( void );
		Location( Location const & obj );
		~Location( void );
		Location	&	operator=( Location const & obj );

	//SETTERS
		void	setDir( std::string dir );
		void	setRoot( std::string root );
		void	setIndex( std::string index );
		void	setRedir( std::string redir );
		void	setListing( std::string listing );
		void	setMethods( std::string method );

	//GETTERS
		std::string					getDir( void );
		std::string					getRoot( void );
		std::string					getIndex( void );
		std::vector<std::string>	getDirectives( void );
		std::string					getRedir( void );
		bool						getListing( void );
		std::vector<std::string>	getMethods( void );		
		
};

#endif