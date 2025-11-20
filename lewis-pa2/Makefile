# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g

OBJS = chash.o hash.o rwlock.o logging.o

chash: $(OBJS)
	$(CC) $(CFLAGS) -o chash $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) chash
