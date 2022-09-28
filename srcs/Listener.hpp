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

#define DEFAULT_PORT 5000

#define LISTEN_BACKLOG 512 // The maximum length for the queue of pending connections.

#pragma once

class Listener
{
private:
	int				fd;
	int				port;

public:
	Listener(int port = DEFAULT_PORT);
	Listener(Listener const &src);

	~Listener();

	Listener	&operator=(Listener const &src);
};
