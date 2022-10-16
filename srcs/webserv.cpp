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
	TOML::Document	config = parse_config_file(argv[0]);
	std::cout << F_BGRN("Config parsed :)") << std::endl;

	std::vector<Listener>	listeners = create_Listeners(config);
	std::cout << "Number of listeners: " << listeners.size() << std::endl;
	std::vector<pthread_t>	threads;
	threads.reserve(listeners.size());

	unsigned int i = 0;
	for (std::vector<Listener>::iterator it = listeners.begin(); it != listeners.end(); ++it, ++i)
	{
		int errnum;
		if ((errnum = pthread_create(&threads[i], NULL, &init_thread, &listeners[i])) < 0)
			throw std::runtime_error(std::string("Failed to launch thread :") + strerror(errnum));
		pthread_detach(threads[i]);
	}

	while (1) {}

	return (0);
}
