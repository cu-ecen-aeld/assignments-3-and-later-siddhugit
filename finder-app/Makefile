CC=$(CROSS_COMPILE)gcc
CFLAGS= -Wall -I.

all: writer_build

writer_build: writer.o
	$(CC) $(CFLAGS) -o writer writer.o 

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf writer