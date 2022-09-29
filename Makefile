# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gphilipp <gphilipp@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/13 15:31:28 by gphilipp          #+#    #+#              #
#    Updated: 2022/09/29 14:47:35 by gphilipp         ###   ########.fr        #
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

CXX  = clang++

CXXFLAGS = -Wall -Wextra -Werror -Wold-style-cast -std=c++98

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
	git submodule update --init $(SMFLAGS) $(TOML_PARSER)

%.o: %.cpp $(HDEP)
	$(CXX) $(CXXFLAGS) -I$(TOML_PARSER) -c $< -o $@

$(NAME): $(OBJ) $(HDEP)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

bonus: $(NAME)_bonus

$(NAME)_bonus: $(NAME)
	cp $(NAME) $(NAME)_bonus

run: all
	./$(NAME)

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
