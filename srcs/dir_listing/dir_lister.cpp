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
#include <sstream>
#include <dirent.h>

#include <webserv_utils.hpp>

#define PUT_DIR_SLASH(x)	(x->d_type == DT_DIR ? "/" : "")
#define LINK(x)				"<a href=\"" << x->d_name << PUT_DIR_SLASH(x) << "\">" \
								<< x->d_name << PUT_DIR_SLASH(x) \
								<< "</a>"

// man readdir, man dirent
std::string	get_dir_list_html(std::string const& dir_path)
{
	std::stringstream	html;

	html << "<!DOCTYPE html>"CRLF;
	html << "<html>"CRLF;
	html << "<head>"CRLF;
	html << 	"<meta charset=\"UTF-8\" />"CRLF;
	html << 	"<title>Index of " << dir_path << "</title>"CRLF;
	html << "</head>"CRLF;
	html << "<body>"CRLF;
	html << 	"<h1>Index of " << dir_path << "</h1>"CRLF;
	html << 	"<hr>"CRLF;
	html << 	"<pre>"CRLF;

	DIR		*dir;
	if ( (dir = opendir(dir_path.c_str())) == NULL )
		html << CRLF;
	else
	{
		for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
		{
			html << LINK(dirent) << CRLF;
		}
		closedir(dir);
	}

	html << 	"</pre>"CRLF;
	html << 	"<hr>"CRLF;
	html << "</body>"CRLF;
	html << "</html>"CRLF;

	return html.str();
}
