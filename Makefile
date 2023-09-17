NAME = nona
CC = gcc
CFLAGS = -g3 -Wall -Wextra -Werror

SRCS = nona_compiler_c/lexer.c nona_compiler_c/string_interning.c nona_compiler_c/hasher.c nona_compiler_c/ast.c  nona_compiler_c/hashmap.c  nona_compiler_c/main.c nona_compiler_c/parser/parser.c
INCLUDE = -Inona_compiler_c -Inona_compiler_c/parser -Inona_compiler_c/vector

OBJS = $(SRCS:.c=.o)


all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@

clean :
	-rm -f $(OBJS)

fclean : clean
	-rm -f $(NAME)

re : fclean $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LDFLAGS) -o $(NAME)

.PHONY: all clean fclean re