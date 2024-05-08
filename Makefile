NAME = webserv

FILES = webserv ServerManager Server Client Request \
		conf/Config conf/parser_utils utils

FILES_SRC = $(addprefix src/, $(FILES))

SRC = $(addsuffix .cpp, $(FILES_SRC))
OBJ = $(addsuffix .o, $(FILES_SRC))


CC = clang++
CFLAGS = #-Wall -Werror -Werror -std=c++98
RM = rm -rf

all: $(NAME)

.c.o: $(SRC)
		@$(CC) $(CFLAGS) -c -o $@ $^

$(NAME): $(OBJ)
		@$(CC) $(CFLAGS) $(SRC) -o $(NAME)
		@echo "Done"

clean:
		@$(RM) $(OBJ)
		@echo "Clean"

fclean: clean
		@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SILENT:% 