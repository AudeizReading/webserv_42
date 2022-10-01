/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   querystring_parser.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gphilipp <gphilipp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 17:15:51 by gphilipp          #+#    #+#             */
/*   Updated: 2022/10/01 10:52:26 by gphilipp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>

#include <ctype.h>
#include <stdlib.h>

typedef std::multimap<std::string, std::string>		multimap_ss;
typedef std::pair<std::string, std::string>			pair_ss;

std::string decode_URI_Component(std::string str)
{
	std::string::iterator		it;
	std::string::iterator		start;
	int							mode = 0;
	int							num;
	char						repl[2] = {0};

	for (it = str.begin(); it != str.end(); it++)
	{
		if (*it == '%')
		{
			start = it;
			mode = 1;
		}
		else if (isxdigit(*it) && 1 <= mode && mode < 3)
		{
			mode++;
			if (mode == 3)
			{
				num = strtol(std::string(start + 1, it + 1).c_str(), NULL, 16);
				if (32 <= num && num <= 126)
				{
					*repl = static_cast<char>(num);
					str.replace(start, it + 1, repl);
					it -= 2;
				}
				else if (0 <= num && num < 32)
				// Securité on accepte pas les caractères invisibles
				{
					str.erase(start, it + 1);
					it -= 3;
				}
				mode = 0;
			}
		}
		else
			mode = 0;
	}
	return (str);
}

void	querystring_parser(multimap_ss &data, const std::string &query, char starter = '?')
{
	char						sep = '&';
	char						eq = '=';

	std::string					varName;
	std::string					key;
	std::string					val;

	std::string::const_iterator	it;
	int							mode = -1;

	for (it = query.begin(); it != query.end(); it++)
	{
		if (mode < 0 && *it == starter && starter != 0)
			continue;
		else if (mode <= 0 && *it == eq)
			mode = 1;
		else if (mode <= 0 && *it != sep && *it != '\0')
			key = ((key == "") ? "" : key) + *it;
		else if (*it != sep && *it != '\0')
			val = val + *it;
		else
		{
			if (key != "")
				data.insert(pair_ss(
					decode_URI_Component(key),
					decode_URI_Component(val)
				));
			key = "";
			val = "";
			mode = 0;
		}
	}
}

int main()
{
	multimap_ss get;

	std::cout << decode_URI_Component("test%20%20test") << std::endl;
	querystring_parser(get, "?salut=20&salut=1&coucou=je%20suis%20guil%00laume&test&salut=30");

	multimap_ss::iterator it;

	std::cout << "multimap " << get.size() << std::endl;

	for (it = get.begin(); it != get.end(); it++)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}

	return 0;
}
