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

#include "webserv.hpp"
#include <toml_parser.hpp>

static bool	_exists_and_has_type(TOML::Value const& group, const char *key, TOML::Type type)
{
	return (group.has(key) && group[key].type() == type);
}

void	check_mandatory_directives(TOML::Document const& doc)
{
	if (!doc.has("http") || !doc["http"].isGroup())
		throw std::runtime_error("Illegal or missing `http' object in configuration file");

	const TOML::Document http(doc["http"]);
	if (!http.has("server") || !http["server"].isArray()
		|| http["server"].Array().type() != TOML::T_GROUP)
		throw std::runtime_error("Illegal or missing `server' block(s) in configuration file");
	
	const TOML::Value::array_type&	serv_array = http["server"].Array();
	for (TOML::Document::array_type::const_iterator it = serv_array.begin();
		it != serv_array.end();
		++it)
	{
		if (!it->has("listen_port") || !(*it)["listen_port"].isInt())
			throw std::runtime_error("missing or illegal `listen_port' directive");
		if (it->has("location"))
		{
			if (!(*it)["location"].isArray() || (*it)["location"].Array().type() != TOML::T_GROUP)
				throw std::runtime_error("illegal `location' directive");

			const TOML::Value::array_type&	locations = (*it)["location"].Array();
			for (TOML::Document::array_type::const_iterator j = locations.begin();
				j != locations.end();
				++j)
			{
				if ( !_exists_and_has_type(*j, "URI", TOML::T_STRING)
					|| !is_valid_URI((*j)["URI"].Str()) )
					throw std::runtime_error("missing or illegal `URI' directive in location");
				if (!_exists_and_has_type(*j, "root", TOML::T_STRING))
					throw std::runtime_error("missing or illegal `root' directive in location");
			}
		}
	}
}

static void	_check_optional_location_directives(TOML::Value::array_type const& locations)
{
	const char			*loc_keys[] = { "index",		"allowed_methods",	"dir_listing",	"redirect"		};
	const TOML::Type	loc_types[] = { TOML::T_STRING,	TOML::T_ARRAY,		TOML::T_BOOL,	TOML::T_STRING	};

	for (TOML::Document::array_type::const_iterator it = locations.begin();
		it != locations.end();
		++it)
	{
		// Types must correspond between keys array and types array
		for (unsigned int i = 0; i < sizeof(loc_keys) / sizeof(loc_keys[0]); ++i)
			if (it->has(loc_keys[i]) && (*it)[loc_keys[i]].type() != loc_types[i])
				throw std::runtime_error(std::string("illegal `") + loc_keys[i] + "' directive in location block");
		
		// Check type of array for allowed_methods
		if (it->has("allowed_methods") && (*it)["allowed_methods"].Array().type() != TOML::T_STRING)
			throw std::runtime_error("illegal `allowed_methods' directive in server block");
	}
}

void	check_optional_directives(TOML::Document const& doc)
{
	const char			*serv_keys[] = {"listen_addr",	"server_name",	"client_max_body_size",	"listen_backlog",	"error_400", 	"error_403",	"error_404"};
	const TOML::Type	serv_types[] = {TOML::T_STRING,	TOML::T_ARRAY,	TOML::T_INT,			TOML::T_INT,		TOML::T_STRING,	TOML::T_STRING,	TOML::T_STRING};

	const TOML::Value::array_type&	serv_array = doc["http"]["server"].Array();
	for (TOML::Document::array_type::const_iterator it = serv_array.begin();
		it != serv_array.end();
		++it)
	{
		// Types must correspond between keys array and types array
		for (unsigned int i = 0; i < sizeof(serv_keys) / sizeof(serv_keys[0]); ++i)
			if (it->has(serv_keys[i]) && (*it)[serv_keys[i]].type() != serv_types[i])
				throw std::runtime_error(std::string("illegal `") + serv_keys[i] + "' directive in server block");
		
		// Check type of array for server_name.
		if (it->has("server_name") && (*it)["server_name"].Array().type() != TOML::T_STRING)
			throw std::runtime_error("illegal `server_name' directive in server block");
		
		// Check each location
		if (it->has("location"))
			_check_optional_location_directives((*it)["location"].Array());
	}
}
