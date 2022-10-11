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

static void	debug_print_recursive(TOML::Document const& doc, std::string ident = "")
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

// This is a recursive, weird mess. But it looks like it works.
static bool	_parse_value(TOML::Value& val);

static bool	_go_through_array(TOML::Value& array)
{
	for (TOML::Document::array_type::iterator j = array.Array().begin();
		j != array.Array().end();
		++j)
	{
		if (_parse_value(*j) == false)
			return false;
	}
	return true;
}

static bool	_parse_value(TOML::Value& val)
{
	if (val.type() == TOML::T_ARRAY)
	{
		return _go_through_array(val);
	}
	else if (val.type() == TOML::T_GROUP)
	{
		if (val.has("include"))
		{
			if (include_directive(val, val["include"]) == false)
				return false;
		}
		for (TOML::Value::group_iterator it = val.group_begin();
			it != val.group_end();
			++it)
		{
			if (_parse_value(*it) == false)
				return false;
		}
	}
	return true;
}

// TODO: Iterate over everything in config file in here, handle special directives like include
TOML::Document	parse_config_file(const char *path)
{
	TOML::Document	config(path);
	config.parse();

	for (TOML::Document::iterator it = config.begin(); it != config.end(); ++it)
	{
		if (_parse_value(*it) == false)
			throw std::runtime_error("Failed to parse configuration file");
	}
	debug_print_recursive(config);
	return config;
}
