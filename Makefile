all: webserv

webserv: webserv.c
	gcc -o webserv $^ -lm

clean:
	rm -f *.o *.so *.a webserv
