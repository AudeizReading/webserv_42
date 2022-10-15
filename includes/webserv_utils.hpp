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

std::string	strtrim_left(std::string const& str, const char *charset = " \r\n\t");
std::string	strtrim_right(std::string const& str, const char *charset = " \r\n\t");
bool		is_valid_URI(std::string const& uri);
