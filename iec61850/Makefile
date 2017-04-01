BINARY_NAME = publisher
SOURCES = publisher.c

INCFLAGS = -I./inc
LDFLAGS = -lpthread
CFLAGS = -Wall -v -g -H
CC = gcc

all:	$(BINARY_NAME)

$(BINARY_NAME):	$(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCFLAGS) -o $(BINARY_NAME) $(SOURCES)

clean:
	rm -f $(BINARY_NAME) $(BINARY_NAME).o
