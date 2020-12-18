# Make file for ease of use

# Variables
PROGS = webserv my_histogram aactrl arduino_connect
OBJS = error.o pathalloc.o

# Compile Everything

all: $(PROGS)

# Compilation Rules

webserv: webserv.c
	gcc -o $@ $^ -lm

my_histogram: my_histogram.c $(OBJS)
	gcc -o $@ $^

aactrl: aactrl.c aactrl.h
	gcc -o $@ $^

arduino_connect: arduino_connect.c
	gcc -o $@ $^

# Objects

error.o: error.c
	gcc -c $^

pathalloc.o: pathalloc.c
	gcc -c $^

# Clean Directory

# *~ : removes all back up files made by vim or emacs
# text after the ":" are the dependencies of the current command
# The actual command starts on the line afterwards preceded by a tab
clean:
	rm -f *.o *~ *.so *.a $(PROGS)

# clean:
#	rm -f *.o *.so *.a webserv
