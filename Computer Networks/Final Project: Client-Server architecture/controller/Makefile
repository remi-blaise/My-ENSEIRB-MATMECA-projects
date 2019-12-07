all:
	gcc -std=c99 -pthread src/*.c -o build/server
	gcc dev/client.c -o build/manual_client
	gcc dev/client2.c -o build/manual_client2
	gcc dev/server.c -o build/manual_server

clean:
	rm build/*
