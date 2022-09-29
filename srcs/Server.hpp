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

public:
	Server();
	Server(std::string root);

	~Server();

	std::string get_root();
};
