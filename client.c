#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include "compression.h"
#include "createmanifest.h"
#include "add.h"
#include "remove.h"
#define MAX 80 

int port;
char host[16];

typedef struct conf {
	char* hostName;
	char* port;
} Configure; 

void sendItem(int sockfd, char* fileName) {
	char sendbuffer[100];
	int b;
	FILE *sp = fopen(fileName, "rb");
	if(sp == NULL){
		perror("File");
		return;
	}

	while( (b = fread(sendbuffer, 1, sizeof(sendbuffer), sp))>0 ){
		send(sockfd, sendbuffer, b, 0);
	}
	printf("Done sending file\n");
	fclose(sp);
	return;
}

void writeItem(int sockfd, char* fileName) {
	printf("Receiving file\n");
	FILE* fp = fopen(fileName, "wb");
	char fileBuff[1024];
	int b, tot = 0;
	while ((b = recv(sockfd, fileBuff, 1024, 0))>0) {
		tot+=b;
		fwrite(fileBuff, 1, b, fp);	
	}
	fclose(fp);
	printf("Received byte: %d\n", tot);
	if (b<0)
		perror("Receiving");
	return;
}

void writeConfigure(char* host, int port) {
	char output[30];
	snprintf(output, sizeof(output), "%s\n%d\0", host, port);

	FILE* out = fopen(".configure", "wb");
	if (!out) {
		printf("Cannot open .configure, exiting\n");
		exit(0);
	}
	fwrite(output, 1, sizeof(output), out);
	fclose(out);
	exit(0);
}

void readConfigure() {
	FILE* in = fopen(".configure", "rb");
	if (!in) {
		printf(".configure not found, loading defaults\n");
		writeConfigure("127.0.0.1", 8000);
		return;
	}

	fscanf(in, "%s\n%d\0", host, &port);
	fclose(in);
}

void execArg(int sockfd, char* instr) {
	if (*instr != '\0') {
		write(sockfd, instr, strlen(instr));
	} else {
		perror("Not a valid arg\n");
		exit(0);	
	}
}

int main(int argc, char** argv) 
{
	if (argc == 1) {
		printf("Client requires arguments, please invoke -h to see all arguments\n");	
		exit(0);	
	}
	if(strcmp(argv[1],"create") == 0){
		/*
		*******NOTE ignore the "manifest" entry in serverManifest and clientManifest***********
		*/
		char folder[100];
		strcpy(folder, argv[2]);
		char placeToPutFolder[100];
		strcpy(placeToPutFolder,"./");
		strcat(placeToPutFolder, folder);
		mkdir(placeToPutFolder, 0777);
		createManifest(folder);
	}
	if(strcmp(argv[1],"add") == 0){
		add(argv[2], argv[3]);
		return 0;
	}
	if(strcmp(argv[1],"remove") == 0){
		removeFile(argv[2],argv[3]);
		return 0;
	}

	if (strcmp(argv[1], "configure") == 0) {
		if (argc < 4) {
			printf("You need to specifiy host and port number, exiting...\n");
			exit(0);	
		}	
		char intbuff[20];	
		writeConfigure(argv[2], atoi(argv[3]));
	}
	readConfigure();
	if (*host == '\0') readConfigure();
	printf("Host:%s\nPort:%d\n", host, port);	

	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("Socket not created\n"); 
		exit(0); 
	} 
	else
		printf("Created socket\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(host); 
	servaddr.sin_port = htons(port); 

	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("Failed to connect to the server\n"); 
		exit(0); 
	} 
	else
		printf("Server connected!\n"); 


	char buff[100];

	//Sends arg to server, example, "commit"	
	execArg(sockfd, argv[1]);	
		
	if (strcmp(argv[1], "push") == 0) {
		compress(argv[2], "./payload.tar.gz");	
	}

	//Infinite loop for client/server messaging
	while (1) {
		int b;
		if ((b = read(sockfd, buff, sizeof(buff))) > 0) {
			if (strcmp(buff, "project") == 0) { 
				printf("Size of argv[2] is %d\n", strlen(argv[2]));
				write(sockfd, argv[2], strlen(argv[2]));
			} 
			if (strcmp(buff, "version") == 0) {
				write(sockfd, argv[3], strlen(argv[3]));	
			} 
			if (strcmp(buff, "filename") == 0) {
				write(sockfd, argv[3], strlen(argv[3]));	
			} 
			if (strcmp(buff, "send") == 0) {
				sendItem(sockfd, "payload.tar.gz");
				remove("payload.tar.gz");
				break;
			} if (strcmp(buff, "receive") == 0) {
				writeItem(sockfd, "recv.tar.gz");
				if (strcmp(argv[1], "upgrade") == 0 || strcmp(argv[1], "checkout") == 0) {
					decompress("recv.tar.gz", ".", 1);
					remove("recv.tar.gz");						
				}
				break;	
			} if (strcmp(buff, "complete") == 0) {
				break;	
			}
			bzero(buff, 100);
		} 
	}	

	close(sockfd); 
} 

