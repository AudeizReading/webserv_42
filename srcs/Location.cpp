/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */

#include "Location.hpp"

Location::Location(std::string const& URI, std::string const& root, std::string const& index,
	bool dir_listing, std::string const& redirect)
: _URI(URI), _root(root), _index(index), _redirect(redirect), _dir_listing(dir_listing),
	_allow_GET(true), _allow_POST(true), _allow_DELETE(true)
{
	if (*(_root.end() - 1) == '/')
		_root.erase(_root.end() - 1);
	if (_URI[0] != '/')
		_URI = '/' + _URI;
}

void	Location::set_allowed_methods(bool GET, bool POST, bool DELETE)
{
	_allow_GET = GET;
	_allow_POST = POST;
	_allow_DELETE = DELETE;
}

bool	Location::allows_GET() const
{
	return _allow_GET;
}
bool	Location::allows_POST() const
{
	return _allow_POST;
}
bool	Location::allows_DELETE() const
{
	return _allow_DELETE;
}

bool	Location::allows_method(std::string const& method_name) const
{
	if (method_name == "GET")
		return _allow_GET;
	else if (method_name == "POST")
		return _allow_POST;
	else if (method_name == "DELETE")
		return _allow_DELETE;
	else
		return false;
}

bool	Location::allows_dir_listing() const
{
	return _dir_listing;
}
bool	Location::has_redirect() const
{
	return _redirect.size();
}

std::string const&	Location::root() const
{
	return _root;
}
std::string const&	Location::URI() const
{
	return _URI;
}
std::string const&	Location::index() const
{
	return _index;
}
std::string const&	Location::redirect() const
{
	return _redirect;
}

std::string		Location::get_path() const
{
	return _root + _URI;
}
