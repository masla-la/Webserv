#include "../inc/Location.hpp"

Location::Location(void)
{
	_directives.push_back("root");
	_directives.push_back("index");
	_directives.push_back("allowed_methods");
	_directives.push_back("dir_listing");
}

Location::Location(Location const &obj)
{
	*this = obj;//
}

Location::~Location(void)
{}

Location	&Location::operator=(Location const &obj)
{
	_dir = obj._dir;
	_root = obj._root;
	_index = obj._index;
	_directives = obj._directives;
	_redir = obj._redir;
	_listing = obj._listing;
	_methods = obj._methods;

	return *this;
}

//SETTERS
void	Location::setDir( std::string dir )
{
	_dir = dir;
}

void	Location::setRoot( std::string root )
{
	_root = root;
}

void	Location::setIndex( std::string index )
{
	_index = index;
	if (index.empty())
		_index = "index.html";
}

void	Location::setRedir( std::string redir )
{
	_redir = redir;
}

void	Location::setListing( std::string listing )
{
	if (!listing.empty() && (listing == "on" || listing == "off"))
		{
			if (listing == "on")
				_listing = true;
			else
				_listing = false;
		}
	else
		throw ListingError();
}

void	Location::setMethods( std::string method )
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw MethodError();
	_methods.push_back(method);
}


//GETTERS
std::string					Location::getDir( void )
{
	return _dir;
}

std::string					Location::getRoot( void )
{
	return _root;
}

std::string					Location::getIndex( void )
{
	return _index;
}

std::vector<std::string>	Location::getDirectives( void )
{
	return _directives;
}

std::string					Location::getRedir( void )
{
	return _redir;
}

bool						Location::getListing( void )
{
	return _listing;
}

std::vector<std::string>	Location::getMethods( void )
{
	return _methods;
}
