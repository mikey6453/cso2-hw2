CC = clang
CFLAGS = -Og -g -std=c11 -Wall -pedantic -fsanitize=address
LDFLAGS = -Wall -fsanitize=address

all: gettimings

gettimings: gettimings.o
	$(CC) $(LDFLAGS) -o $@ $<

gettimings.o: gettimings.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f gettimings.o gettimings
