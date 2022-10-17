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

typedef std::map<std::string, std::string>			t_map_ss;

extern t_map_ss	*mime_types;

int				webserv(int argc, char *argv[]);

TOML::Document	parse_config_file(const char *path);
void			include_directive(TOML::Value& target, TOML::Value include);
void			check_mandatory_directives(TOML::Document const& doc);
void			check_optional_directives(TOML::Document const& doc);

// class Listener;
std::vector<class Listener>	create_Listeners(TOML::Document const& conf);
