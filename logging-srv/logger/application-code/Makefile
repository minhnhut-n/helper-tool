# Environment declaration first
CC = gcc
CFLAGS = -Wall -Wextra -pthread -I.

# next, output file name are specified. <name>.o
APP_SRC = app-code.c logger-srv.c
APP_OBJS = $(APP_SRC:.c=.o)

# the output daemon executable name
APP_TARGET = app

all: $(APP_TARGET)

# daemon compile,
# "$@" represent for target, is left-side ":"
# "$^" represent for depen.., is right-side ":"
$(APP_TARGET): $(APP_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# compile .c to .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean up
clean:
	rm -f *.o $(APP_TARGET)

.PHONY: all clean