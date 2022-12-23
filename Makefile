NAME = nona
CC = gcc
CFLAGS = -g3

SRCS = nona_compiler_c/lexer.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LDFLAGS) -o $(NAME)