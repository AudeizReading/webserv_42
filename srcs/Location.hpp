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

#pragma once

#include <string>
#include <vector>
#include "Request.hpp"

class Location
{
	public:
		Location(std::string const& URI, std::string const& root, std::string const& index,
			bool dir_listing, std::string const& redirect)
		: _URI(URI), _root(root), _index(index), _redirect(redirect), _dir_listing(dir_listing),
			_allow_GET(true), _allow_POST(true), _allow_DELETE(true)
		{
			if (*(_root.end() - 1) == '/')
				_root.erase(_root.end() - 1);
			if (_URI[0] != '/')
				_URI = '/' + _URI;
		}

		void	set_allowed_methods(bool GET, bool POST, bool DELETE)
		{
			_allow_GET = GET;
			_allow_POST = POST;
			_allow_DELETE = DELETE;
		}
	
	public:
		bool	allows_GET() const			{ return _allow_GET;		}
		bool	allows_POST() const			{ return _allow_POST;		}
		bool	allows_DELETE() const		{ return _allow_DELETE;		}
		bool	allows_dir_listing() const	{ return _dir_listing;		}
		bool	has_redirect() const		{ return _redirect.size();	}

		std::string const&	root() const		{ return _root;		}
		std::string const&	URI() const			{ return _URI;		}
		std::string const&	index() const		{ return _index;	}
		std::string const&	redirect() const	{ return _redirect;	}

		std::string		get_path() const	{ return _root + _URI; }

	private:
		std::string		_URI;	// Must start with a '/'
		std::string		_root;	// Must not end with a '/'
		std::string		_index;
		std::string		_redirect; // Empty string means no redirect, else, redirect 302

		int8_t			_dir_listing;
		int8_t			_allow_GET;		// These 4 are declared as int8 and not
		int8_t			_allow_POST;	// bool to save space. Combined, they
		int8_t			_allow_DELETE;	// take the space of a single bool.
		// NOTE: Include some CGI stuff in there ?
};
