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

#include <string>

std::string	strtrim_right(std::string const& str, const char *charset)
{
	std::string::size_type	tgt = str.find_last_not_of(charset);

	if (tgt == std::string::npos)
		return "";
	else
		return str.substr(0, tgt + 1);
}

std::string	strtrim_left(std::string const& str, const char *charset)
{
	std::string::size_type	tgt = str.find_first_not_of(charset);

	if (tgt == std::string::npos)
		return "";
	else
		return str.substr(tgt);
}
