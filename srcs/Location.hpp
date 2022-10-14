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
			bool dir_listing, std::string const& redirect);

		void	set_allowed_methods(bool GET, bool POST, bool DELETE, bool HEAD);
	
	public:
		bool		allows_GET() const;
		bool		allows_POST() const;
		bool		allows_DELETE() const;
		bool		allows_HEAD() const;
		// Returns whether a given method is allowed. Given string has to be all uppercase, no whitespaces.
		bool		allows_method(std::string const& method_name) const;
		// Returns a string of allowed methods in the RFC expected format for the "Allow" field.
		std::string	get_allowed_methods() const;

		bool	allows_dir_listing() const;
		bool	has_redirect() const;

		std::string const&	root() const;
		std::string const&	URI() const;
		std::string const&	index() const;
		std::string const&	redirect() const;

		std::string			get_path() const;

	private:
		std::string		_URI;	// Must start with a '/'
		std::string		_root;	// Must not end with a '/'
		std::string		_index;
		std::string		_redirect; // Empty string means no redirect, else, redirect 302

		int8_t			_dir_listing;
		int8_t			_allow_GET;		// These 4 are declared as int8 and not
		int8_t			_allow_POST;	// bool to save space. Combined, they
		int8_t			_allow_DELETE;	// take the space of a single bool.
		int8_t			_allow_HEAD;
		// NOTE: Include some CGI stuff in there ?
};
