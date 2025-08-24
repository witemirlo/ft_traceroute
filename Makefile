NAME     := ft_traceroute

SRCS     := src/main.c\
	    src/connection_data.c\
	    src/icmp.c\
	    src/init.c\
	    src/routine.c\
	    src/utils.c

OBJS     := $(SRCS:.c=.o)
DEPS     := $(SRCS:.c=.d)

CC       := gcc
CFLAGS   := -Wall -Wextra -O0 -g3
CPPFLAGS := -I include/

# ------------------------------------------------------------------------------

all: $(NAME)
re: fclean all

$(NAME): $(OBJS) include/ft_traceroute.h
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	make set_capabilities

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

set_capabilities:
	sudo setcap 'cap_net_raw=ep' $(shell pwd)/${NAME}

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

.SECONDARY: $(OBJS)
.PHONY: all clean fclean re set_capabilities
