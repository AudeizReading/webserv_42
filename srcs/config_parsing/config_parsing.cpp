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

// This is a recursive, weird mess. But it looks like it works.
static void	_parse_value(TOML::Value& val);

static void	_go_through_array(TOML::Value& array)
{
	for (TOML::Document::array_type::iterator j = array.Array().begin();
		j != array.Array().end();
		++j)
	{
		_parse_value(*j);
	}
}

// Does whatever is required with specific keys. Right now, only the "include" directive matters.
static void	_parse_value(TOML::Value& val)
{
	if (val.type() == TOML::T_ARRAY)
	{
		return _go_through_array(val);
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
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error(std::string("Failed to parse configuration file: ") + e.what());
	}

	// debug_print_recursive(config);
	return config;
}
