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

// TODO: Iterate over everything in config file in here, handle special directives like include
TOML::Document	parse_config_file(const char *path)
{
	TOML::Document	config(path);
	config.parse();

	// NOTE: Temporary hardcode for now. Iterate over every key in every group?
	// TODO: Agree on a config file standard.
	if (config.at("http").has("include"))
	{
		include_directive(config["http"], config["http"]["include"]);
		// for (TOML::Document::iterator it = config.begin(); it != config.end(); ++it)
		// {
		// 	std::cout << BLUHB << *it << RESET << std::endl << std::endl;
		// }
	}
	return config;
}
