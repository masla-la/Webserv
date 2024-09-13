NAME = webserv

FILES = webserv ServerManager Server Client Request \
		conf/Config conf/parser_utils conf/Location utils \
		cgi

FILES_SRC = $(addprefix src/, $(FILES))

SRC = $(addsuffix .cpp, $(FILES_SRC))
OBJ = $(addsuffix .o, $(FILES_SRC))

CC = clang++
CFLAGS = -Wall -Werror -Wextra -std=c++98
RM = rm -rf

all: $(NAME)

$(OBJ): %.o: %.cpp
	@$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $^ -o $(NAME)
	@echo "Done"

clean:
	@$(RM) $(OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
