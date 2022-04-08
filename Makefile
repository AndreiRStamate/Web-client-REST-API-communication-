CC=g++
CFLAGS=-I.

client: client.cc requests.cc helpers.cc buffer.cc 
	$(CC) -o client client.cc requests.cc helpers.cc buffer.cc -Wall

run: client
	./client

clean:
	rm -f *.o client
