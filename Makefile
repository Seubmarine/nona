NAME = nona
CC = gcc
CFLAGS = -g3 -Wall -Wextra -Werror

SRCS = nona_compiler_c/lexer.c nona_compiler_c/string_interning.c
INCLUDE = -Inona_compiler_c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

clean :
	-rm -f $(OBJS)

fclean : clean
	-rm -f $(NAME)

re : fclean $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LDFLAGS) -o $(NAME)

.PHONY: all clean fclean re