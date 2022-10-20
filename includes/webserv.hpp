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

#include <iostream>
#include <string>
#include <exception>
#include <cassert>
#include <map>

#include <toml_parser.hpp>
#include <webserv_utils.hpp>

#define DEFAULT_CONFIG_FILE	"./demo/www.toml"

typedef std::map<std::string, std::string>			t_map_ss;

extern t_map_ss	*mime_types;

int				webserv(const char *config_file_path);

TOML::Document	parse_config_file(const char *path);
void			include_directive(TOML::Value& target, TOML::Value include);
void			check_mandatory_directives(TOML::Document const& doc);
void			check_optional_directives(TOML::Document const& doc);

// class Listener;
std::vector<class Listener>	create_Listeners(TOML::Document const& conf);
