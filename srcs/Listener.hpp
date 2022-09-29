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

#include <toml_parser.hpp>

#define DEFAULT_PORT 5000

#define LISTEN_BACKLOG 512 // The maximum length for the queue of pending connections.

#pragma once

class Listener
{
private:
	int		_fd; // Il faut qu'on adopte une convention sur le nom des variables d'une classe.
	int		_port; // Il y en a plusieurs : commencer par "m_", "_", finir par "_", etc.
	int		_listen_backlog; // Mais faut différencier les membres private/protected des publics.

public:
	Listener(int port = DEFAULT_PORT);
	Listener(TOML::Document const& config);
	Listener(Listener const &src);

	void	test_start();

	~Listener();

	Listener	&operator=(Listener const &src);
};
