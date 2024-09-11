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
	@make -C ./utils/Get_next_line/
	@mv ./utils/Get_next_line/get_next_line.a ./src/
	@$(CC) $(CFLAGS) $^ ./src/get_next_line.a -o $(NAME)
	@echo "Done"

clean:
	@$(RM) $(OBJ)
	@make clean -C ./utils/Get_next_line/
	@echo "Clean"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) ./src/get_next_line.a
	@make fclean -C ./utils/Get_next_line/

re: fclean all

.PHONY: all clean fclean re
