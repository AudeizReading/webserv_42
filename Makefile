# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/13 15:31:28 by gphilipp          #+#    #+#              #
#    Updated: 2022/10/05 23:21:10 by pbremond         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

PARSING_SRC_FILES =	include_directive.cpp		config_parsing.cpp

PARSING_SRC = $(addprefix srcs/config_parsing/, $(PARSING_SRC_FILES))

# <!-- pre='./' path='srcs/' match='*.cpp' exclude='srcs/main.cpp' pos='1' template='		{0} \' -->
SRC =	srcs/main.cpp \
		srcs/Response/Response_4XX.cpp \
		srcs/Response/Response_Ok.cpp \
		srcs/Listener.cpp \
		srcs/Request.cpp \
		srcs/Response.cpp \
		srcs/Server.cpp \
		srcs/webserv.cpp \
		srcs/CGIManager.cpp \
		$(PARSING_SRC)

# <!-- pre='./' path='srcs/' match='*.hpp' exclude='srcs/webserv.hpp' pos='1' template='		{0} \' -->
HDEP  = srcs/Response/Response_4XX.hpp \
		srcs/Response/Response_Ok.hpp \
		srcs/Listener.hpp \
		srcs/Request.hpp \
		srcs/Response.hpp \
		srcs/Server.hpp \
		srcs/CGIManager.hpp \

TOML_PARSER = lib/toml_parser

INCLUDES = $(TOML_PARSER) includes

OBJ = $(subst srcs/, objs/, $(patsubst %.cpp, %.o, $(SRC)))

NAME = webserv

CXX  = clang++

CXXFLAGS = -Wall -Wextra -Werror -Wold-style-cast -g -std=c++98

CONF_FILE = demo/www.toml

ifeq ($(shell ./hooks/submodules > /dev/null; echo $$?), 1)
LIBS = toml
endif

ifdef TR
SMFLAGS = --remote
LIBS = toml
endif

all: libs $(NAME)

libs: $(LIBS)

toml:
#	git submodule update --init $(SMFLAGS) $(TOML_PARSER)

objs/%.o: srcs/%.cpp $(HDEP)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(TOML_PARSER) -Iincludes -c $< -o $@

$(NAME): $(OBJ) $(HDEP)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

bonus: $(NAME)_bonus

$(NAME)_bonus: $(NAME)
	cp $(NAME) $(NAME)_bonus

run: all
	./$(NAME) $(CONF_FILE)

# Usage: make debug && lldb minishell_debug -o run
$(NAME)_debug: $(SRC) $(HDEP)
	$(CXX) $(CXXFLAGS) -g -o $(NAME)_debug $(SRC)

$(NAME)_sanitize: $(SRC) $(HDEP)
	$(CXX) $(CXXFLAGS) -fsanitize=address -g -o $(NAME)_sanitize $(SRC)

debug: $(NAME)_debug

sanitize: $(NAME)_sanitize

clean:
	rm -f $(OBJ)

test:
	@./test/test.sh

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME)_bonus $(NAME)_debug $(NAME)_sanitize
	rm -f *.out test/*.out
	rm -f *.log
	rm -rf *.dSYM
	find . -iname "*.o" -exec rm -i {} ";"

re: fclean all

.PHONY: all libs toml run debug sanitize clean fclean re test
