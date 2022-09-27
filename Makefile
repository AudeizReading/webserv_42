# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gphilipp <gphilipp@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/13 15:31:28 by gphilipp          #+#    #+#              #
#    Updated: 2022/09/27 23:00:59 by gphilipp         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# <!-- pre='./' path='srcs/' match='*.cpp' exclude='srcs/main.cpp' pos='1' template='		{0} \' -->
SRC   = srcs/main.cpp \
		srcs/Listener.cpp \
		srcs/webserv.cpp \


# <!-- pre='./' path='srcs/' match='*.hpp' exclude='srcs/webserv.hpp' pos='1' template='		{0} \' -->
HDEP  = srcs/webserv.hpp \
		srcs/Listener.hpp \

TOML_PARSER = lib/toml_parser

OBJ = $(SRC:.cpp=.o)

NAME = webserv

CXX  = g++

CXXFLAGS = -Wall -Wextra -Werror -Wold-style-cast -std=c++98

all: libs $(NAME)

libs: toml_parser

$(TOML_PARSER):
	git submodule update --init $(TOML_PARSER)

toml_parser: $(TOML_PARSER)

%.o: %.cpp $(HDEP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ) $(HDEP)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

bonus: $(NAME)_bonus

$(NAME)_bonus: $(NAME)
	cp $(NAME) $(NAME)_bonus

run: all
	./$(NAME)

# Usage: make debug && lldb minishell_debug -o run
$(NAME)_debug: $(SRCS) $(HDEPS)
	$(CXX) $(CXXFLAGS) -g -o $(NAME)_debug $(SRCS)

$(NAME)_sanitize: $(SRCS) $(HDEPS)
	$(CXX) $(CXXFLAGS) -fsanitize=address -g -o $(NAME)_sanitize $(SRCS)

debug: $(NAME)_debug

sanitize: $(NAME)_sanitize

clean:
	rm -f $(OBJ)

test:
	@./test/test.sh

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME)_bonus
	rm -f *.out test/*.out
	rm -f *.log
	find . -iname "*.o" -exec rm -i {} ";"

re: fclean all

.PHONY: all libs toml_parser run debug sanitize clean fclean re test
