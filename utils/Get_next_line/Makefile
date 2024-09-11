SRC = get_next_line.c get_next_line_utils.c
SRC-BONUS = get_next_line_bonus.c get_next_line_utils_bonus.c

OBJ = $(SRC:%.c=%.o)
OBJ-BONUS = $(SRC-BONUS:%.c=%.o)

NAME = get_next_line.a

CC = gcc
CFLAGS = -Wextra -Wall -Werror -D BUFFER_SIZE=1 $(SRC)
LIBC = ar rcs

RM = rm -f

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(NAME): $(OBJ)
	$(LIBC) $@ $^

bonus: $(OBJ-BONUS)
	$(LIBC) $(NAME) $^

clean:
	$(RM) $(OBJ) $(OBJ-BONUS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re
