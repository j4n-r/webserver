CC = gcc
CFLAGS = -Wall -Wextra -g

server: server.o utils.o endpoints.o
	$(CC) $(CFLAGS) webserver/out/server.o webserver/out/utils.o webserver/out/endpoints.o -o webserver/server 

server.o: webserver/server.c
	$(CC) $(CFLAGS) -c webserver/server.c -o webserver/out/server.o

utils.o: webserver/utils.c
	$(CC) $(CFLAGS) -c webserver/utils.c -o webserver/out/utils.o

endpoints.o: webserver/endpoints.c
	$(CC) $(CFLAGS) -c webserver/endpoints.c -o webserver/out/endpoints.o
run: 
	./webserver/server

clean:
	rm -f webserver/server.o webserver/server webserver/utils.o 

