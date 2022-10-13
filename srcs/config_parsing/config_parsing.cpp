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

void	debug_print_recursive(TOML::Document const& doc, std::string ident = "")
{
	for (TOML::Document::const_iterator it = doc.begin(); it != doc.end(); ++it)
	{
		if (it->isGroup()) {
			std::cout << ident + '\t' << "[" + it->key() + "]" << std::endl;
			debug_print_recursive(TOML::Document(*it), ident + '\t');
		}
		else if (it->isArray() && it->Array().type() == TOML::T_GROUP)
		{
			for (TOML::Document::array_type::const_iterator arr_it = it->Array().begin();
				arr_it != it->Array().end();
				++arr_it)
			{
				std::cout << ident + '\t' << "[[" + it->key() + "]]" << std::endl;
				debug_print_recursive(TOML::Document(*arr_it), ident + '\t');
				std::cout << std::endl;
			}
		}
		else
			std::cout << ident << it->key() << " = " << *it << std::endl;
	}
}

// A bit of a recursive, weird mess. But it looks like it works.
// Does whatever is required with specific keys. Right now, only the "include" directive matters.
static void	_parse_value(TOML::Value& val)
{
	if (val.type() == TOML::T_ARRAY)
	{
		// for (auto it : val.Array()) => Remember what they took away from you.
		for (TOML::Document::array_type::iterator it = val.Array().begin();
			it != val.Array().end();
			++it)
		{
			_parse_value(*it);
		}
	}
	else if (val.type() == TOML::T_GROUP)
	{
		if (val.has("include"))
			include_directive(val, val["include"]);
		for (TOML::Value::group_iterator it = val.group_begin();
			it != val.group_end();
			++it)
		{
			_parse_value(*it);
		}
	}
}

// Goes through each server block in the HTTP group, and adds a default "location"
// to a server when none is defined.
static void	_insert_default_locations(TOML::Value& http)
{
	TOML::Value::array_type&	serv_array = http["server"].Array();
	for (TOML::Document::array_type::iterator it = serv_array.begin();
		it != serv_array.end();
		++it)
	{
		if (!it->has("location"))
		{
			TOML::Value new_group = TOML::make_group("");
			new_group.group_addValue( TOML::make_string("URI", "/") );
			new_group.group_addValue( TOML::make_string("root", "/var/www") );
			it->group_addValue( TOML::make_array("location", TOML::T_GROUP) );
			(*it)["location"].groupArray_addValue(new_group);
		}
	}
}

// Opens, parses, and does some basic error checking on the configuration file.
// Checks if mandatory directives are present. Checks if each supported directive has
// got the right type. Does NOT check for directives with the right type but bad values.
// Throws a descriptive std::runtime_error if something went wrong.
TOML::Document	parse_config_file(const char *path)
{
	TOML::Document	config(path);
	try
	{
		config.parse();
		for (TOML::Document::iterator it = config.begin(); it != config.end(); ++it)
			_parse_value(*it);
		check_mandatory_directives(config);
		check_optional_directives(config);
		_insert_default_locations(config["http"]);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error(std::string("Failed to parse configuration file: ") + e.what());
	}

	// debug_print_recursive(config);
	// std::cout << config["http"].at("mime_types") << std::endl;
	return config;
}
