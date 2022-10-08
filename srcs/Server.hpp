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

class Server
{
private:
	std::string		_root;
	std::string		_name;
	std::string		_domain;

public:
	Server(std::string root, std::string name, std::string domain);

	~Server();

	std::string get_root() const;
	std::string get_name() const;
	std::string get_domain() const;
};
