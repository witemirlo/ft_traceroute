NAME     := ft_traceroute

SRCS     := src/main.c\
	    src/connection_data.c\
	    src/icmp.c\
	    src/init.c\
	    src/udp.c\
	    src/utils.c

OBJS     := $(SRCS:.c=.o)
DEPS     := $(SRCS:.c=.d)

CC       := gcc
CFLAGS   := -Wall -Wextra -O0 -g3 -pedantic -Wformat=2 -Wformat-overflow=2 -fsanitize=address,pointer-compare,pointer-subtract,leak,undefined,float-divide-by-zero,float-cast-overflow
CPPFLAGS := -I include/

VFLAGS   := -Wall -Wextra -O0 -g3 -pedantic -Wformat=2 -Wformat-overflow=2
TFLAGS   := google.com

TTL      := 64

# ------------------------------------------------------------------------------

all: $(NAME)
re: fclean all

$(NAME): $(OBJS) include/ft_traceroute.h
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
ifneq ($(CFLAGS), $(VFLAGS))
	make set_capabilities
endif

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

valgrind:
	make re CFLAGS="$(VFLAGS)"
	sudo valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --trace-children=yes ./$(NAME) $(TFLAGS)

set_capabilities:
	sudo setcap 'cap_net_raw=ep' $(shell pwd)/${NAME}

set_ttl:
	sudo sysctl -w net.ipv4.ip_default_ttl=$(TTL)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

.SECONDARY: $(OBJS)
.PHONY: all clean fclean re set_capabilities valgrind
