SRC_NAME = main.c \
	strsplit.c \
	conv.c \
	solve.c \
	hashmap.c \
	utils.c \
	rotation.c \
	get_id.c

NAME = rubik

CC = gcc

CFLAGS = -Wall -Wextra -Werror -I./header -O3

SRCF = ./src/

OBJF = ./obj/

OBJS = $(addprefix $(OBJF), $(SRC_NAME:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(CFLAGS)

$(OBJF)%.o: $(SRCF)%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) -c $(addprefix $(SRCF), $*.c) -O3

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
