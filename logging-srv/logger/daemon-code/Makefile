# Environment declaration first
CC = gcc
CFLAGS = -Wall -Wextra -pthread -I.

# next, output file name are specified. <name>.o
DAEMON_SRCS = logger-daemon.c ring_buffer.c
DAEMON_OBJS = $(DAEMON_SRCS:.c=.o)

# the output daemon executable name
DAEMON_TARGET = logger_daemon

all: $(DAEMON_TARGET)

# daemon compile,
# "$@" represent for target, is left-side ":"
# "$^" represent for depen.., is right-side ":"
$(DAEMON_TARGET): $(DAEMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# compile .c to .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean up
clean:
	rm -f *.o $(DAEMON_TARGET)

.PHONY: all clean