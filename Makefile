all: compress delete client server

delete: deleteDir.c deleteDir.h
	gcc -c deleteDir.c deleteDir.h

compress: compression.c compression.h
	gcc -c compression.c compression.h

server: server.c compression.o deleteDir.o
	gcc -lcrypto -pthread -o WTFServer server.c compression.o deleteDir.o

client: client.c compression.o
	gcc -lcrypto -o WTF client.c compression.o


clean:
	rm -rf WTF
	rm -rf WTFServer
	rm -rf deleteDir.o deleteDir.h.gch
	rm -rf compression.o compression.h.gch
