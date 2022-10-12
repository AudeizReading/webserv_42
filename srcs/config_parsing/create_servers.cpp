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

#include "webserv.hpp"
#include "../Listener.hpp"
#include "../Server.hpp"
#include <toml_parser.hpp>

#include <vector>
#include <string>
#include <algorithm>

/* static */ void	_get_servers_from_config(TOML::Value::array_type const& toml_servers,
	std::vector<Server>& servers)
{
	for (TOML::Value::array_type::const_iterator it = toml_servers.begin();
		it != toml_servers.end();
		++it)
	{
		std::vector<std::string>	server_names;
		if (!it->has("server_name"))
			server_names.push_back("");
		else
		{	// Copy everything in there. Assign won't work because TOML::Value isn't 
			// implicitely convertible to string. TODO ?
			for (TOML::Value::array_type::const_iterator j = (*it)["server_names"].Array().begin();
				j != (*it)["server_names"].Array().end();
				++j)
			{
				server_names.push_back(j->Str());
			}
		}
		servers.push_back(
			Server(
				it->at_or("listen_addr",			TOML::make_string("0.0.0.0"))	.Str(),
				it->at_or("client_max_body_size",	TOML::make_int(1048576))		.Int(),
				(*it)["port"].Int(),
				server_names.begin(),
				server_names.end()
			) );
	}
}

// NOTE: I don't know which is the best, in term of performance and/or clean code ?
// Apparently this might take advantage of RVO.
static std::vector<Server>	_get_servers_from_config(TOML::Value::array_type const& toml_servers)
{
	std::vector<Server>				servers;

	for (TOML::Value::array_type::const_iterator it = toml_servers.begin();
		it != toml_servers.end();
		++it)
	{
		std::vector<std::string>	server_names;
		if (!it->has("server_name"))
			server_names.push_back("");
		else
		{	// Copy everything in there. Assign won't work because TOML::Value isn't 
			// implicitely convertible to string. TODO ?
			for (TOML::Value::array_type::const_iterator j = (*it)["server_names"].Array().begin();
				j != (*it)["server_names"].Array().end();
				++j)
			{
				server_names.push_back(j->Str());
			}
		}
		servers.push_back(
			Server(
				it->at_or("listen_addr",			TOML::make_string("0.0.0.0"))	.Str(),
				it->at_or("client_max_body_size",	TOML::make_int(1048576))		.Int(),
				(*it)["port"].Int(),
				server_names.begin(),
				server_names.end()
			) );
	}
	return servers;
}

static bool	_Server_port_compare(Server const& a, Server const& b)
{
	return a.get_port() < b.get_port();
}

static bool	_Server_different_port(Server const& a, Server const& b)
{
	return a.get_port() != b.get_port();
}

#define CEND(__type, x)		(__type::const_iterator(x.end()))
#define VEC_CEND(__T, x)	(std::vector<__T>::const_iterator(x.end()))

// Should return a vector of Listeners, each fully configured AND error checked
// TOML::Document as an input is already parsed and error checked for missing/incorrect directives
std::vector<Listener>	create_Listeners(TOML::Document const& conf)
{
	TOML::Value::array_type const&	toml_servers = conf["http"]["servers"].Array();
	// TODO: Server locations
	std::vector<Server>				servers = _get_servers_from_config(toml_servers);
	std::vector<Listener>			listeners;

	// Sorting servers by port number allows us to give iterator ranges to Listener later on.
	std::sort(servers.begin(), servers.end(), _Server_port_compare);

	for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end();)
	{
		int	port = it->get_port();
		std::vector<Server>::const_iterator	different_port
			= std::find_first_of(it, VEC_CEND(Server, servers), it, it, _Server_different_port);
		listeners.push_back(Listener(port, LISTEN_BACKLOG, it, different_port));
		it = different_port;
	}

	return listeners;
}
