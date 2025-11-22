CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Object files
OBJS = chash.o rwlock.o

# Final program name
TARGET = chash

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Clean
clean:
	rm -f $(TARGET) $(OBJS) hash.log
