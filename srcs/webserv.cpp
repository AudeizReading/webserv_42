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

#include <iostream>

#include <webserv.hpp>
#include "Listener.hpp"

#include <csignal>
#include <iostream>
#include <stdio.h>
#include <pthread.h>

void signal_handler(int signal)
{
	(void) signal;
	exit(0);
}

static void	*init_thread(void *listener)
{
	reinterpret_cast<Listener *>(listener)->start_listener();
	return (0);
}

// NOTE: argc and argv are offset by -1. argv[0] is the first argument,
// and argc == 0 means no arguments.
int	webserv(int argc, char *argv[])
{	
	if (argc != 1)
	{
		std::cerr << "Error: missing configuration file path" << std::endl;
		return 1;
	}
	std::signal(SIGINT, signal_handler);

	std::cout << "Config file: " << argv[0] << std::endl;
	TOML::Document	config = parse_config_file(argv[0]).at("listener");

	Listener	*listeners[10];
	pthread_t	threads[10];

	int i = 0;
	for (TOML::Document::iterator it = config.begin(); it != config.end(); ++it, ++i)
	{
		listeners[i] = new Listener(*it);
		if (pthread_create(&threads[i], NULL, &init_thread, listeners[i]) < 0)
			perror("thread: ");
		pthread_detach(threads[i]);
	}
	while (1) {}
	return (0);
}
