CC=gcc

all: Client.c Server.c
	$(CC) -o Client Client.c -w
	$(CC) -o Server Server.c -w

all-Run: all
	./Server 25561&
	./Client localhost 25561

close: 
	-pkill -f Server

clean: 
	rm -f Client Server
    
