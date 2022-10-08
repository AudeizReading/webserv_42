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

public:
	Server(std::string root, std::string name);

	~Server();

	std::string get_root() const;
	std::string get_name() const;
};
