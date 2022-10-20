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

int			looking_for_iceberg = 1;
t_map_ss	*mime_types = NULL;

void signal_handler(int signal)
{
	(void) signal;
	looking_for_iceberg = 0;
}

static void	*init_thread(void *listener)
{
	reinterpret_cast<Listener *>(listener)->start_listener();
	return (0);
}

static bool create_dico_mimetypes(TOML::Document config)
{
	if (!config.has("mime_types"))
	{
		std::cerr << "Error: add mime_types dico." << std::endl;
		return (false);
	}

	TOML::Document	conf = config.at("mime_types");

	mime_types = new t_map_ss();
	for (TOML::Document::iterator it = conf.begin(); it != conf.end(); ++it)
		// TODO: Parse forbidden char ???
		mime_types->insert(std::pair<std::string, std::string>(it->key(), it->Str()));
	return (true);
}

int	webserv(const char *config_file_path)
{
	if (config_file_path == NULL)
		config_file_path = DEFAULT_CONFIG_FILE;

	std::signal(SIGINT, signal_handler);

	std::cout << "Config file: " << config_file_path << std::endl;
	TOML::Document	config = parse_config_file(config_file_path);
	std::cout << F_BGRN("Config parsed :)") << std::endl;

	if (!create_dico_mimetypes(config.at("http")))
		return (1);

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

	while (looking_for_iceberg) {}

	return (0);
}
