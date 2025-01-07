CC=$(CROSS_COMPILE)gcc
CFLAGS= -Wall -Werror -I.

all: aesdsocket_build

aesdsocket_build: aesdsocket.o
	$(CC) $(CFLAGS) -o aesdsocket aesdsocket.o 

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf aesdsocket