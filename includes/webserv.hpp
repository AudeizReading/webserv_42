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
#include <toml_parser.hpp>

int				webserv(int argc, char *argv[]);
TOML::Document	parse_config_file(const char *path);
bool			include_directive(TOML::Value& target, TOML::Value include);
