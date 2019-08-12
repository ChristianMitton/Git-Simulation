#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include "createmanifest.h"
#include "compression.h"
#include "add.h"
#include "createupdate.h"
#include "upgrade.h"
#include "createcommit.h"

#define MAX 256
#define MAX_BOUND 30

pthread_mutex_t mLock = PTHREAD_MUTEX_INITIALIZER;

void sendItem(int sockfd, char* fileName) {
	char sendbuffer[1024];
	int b;
	FILE *fp = fopen(fileName, "rb");
	if (fp == NULL) {
		perror("File");
		return;	
	}
	while ((b = fread(sendbuffer, 1, sizeof(sendbuffer), fp))>0) {
		send(sockfd, sendbuffer, b, 0);	
	}
	printf("Done sending file\n");
	fclose(fp);
	return;
}

void writeFile(int sockfd, char* fileName) {
	FILE* fp = fopen(fileName, "wb");
	char fileBuff[1024];
	int b, tot = 0;
	while( (b = read(sockfd, fileBuff, 1024))> 0 ) {
		tot+=b;
		fwrite(fileBuff, 1, b, fp);
	}
	printf("Received byte: %d\n",tot);
	fclose(fp);		
	return;
}

void getProj(int sockfd, char* buff) {
	write(sockfd, "project", sizeof("project")); 
	bzero(buff, MAX);
	read(sockfd, buff, MAX);
}

void getFile(int sockfd, char* buff){
	write(sockfd, "filename", sizeof("filename"));
	bzero(buff, MAX);
	read(sockfd, buff, MAX);
}

void getVersion(int sockfd, char* buff) {
	bzero(buff, MAX);
	write(sockfd, "version", sizeof("version")); 
	read(sockfd, buff, MAX);
}

void* exec(void* arg)
{ 
	int sockfd = *((int*)arg);
	char buff[MAX]; 
	int n; 
		
	pthread_mutex_lock(&mLock);
	bzero(buff, MAX); 
	
	read(sockfd, buff, sizeof(buff)); 
	if (strcmp(buff, "commit") == 0)  {
		printf("Trying to commit file\n");

		getProj(sockfd, buff);
		printf("New buff is %s\n", buff);

		createCommit(buff);
		write(sockfd, "complete", sizeof("complete"));
		/*
		bzero(buff, MAX);
		write(sockfd, "send", strlen("send")); 
		
		writeFile(sockfd, "recv.tar.gz");
		*/
	}
	if (strcmp(buff, "receive") == 0)  {
		printf("Trying to send file\n");
		bzero(buff, MAX);
		write(sockfd, "receive", sizeof("receive")); 
		sendItem(sockfd, "recv.tar.gz"); 		
	} 
	if (strcmp(buff, "send") == 0) {
		write(sockfd, "send", sizeof("send"));
		writeFile(sockfd, "recv.tar.gz");	
	}
	

	/*---------------
	Create is working
	-----------------*/
	if (strcmp(buff, "create") == 0) {
		char newDirectory[1048];
		getProj(sockfd, newDirectory);
		printf("directory size is %d\n", strlen(newDirectory));
		printf("Trying to create proj, %s\n", newDirectory);
	
		//get "./server_repos/" path
		char server_repos[100];
		strcpy(server_repos, "./server_repos/");
		
		//append "<projectName>_repo/" to "./server_repos/"
		char projectName_repo[100];
		strcpy(projectName_repo, server_repos);
		strcat(projectName_repo, newDirectory);
		strcat(projectName_repo, "_repo");
	
		//create "./server_repos/<projectName>_repo" folder
		int res = mkdir(projectName_repo, 0777);
		if (res < 0) {
			write(sockfd, "complete", sizeof("complete"));
			printf("Unable to create directory\n");
		}

		//create "./server_repos/<projectName>_repo/<projectName>" folder
		char projectLocation[100];
		strcpy(projectLocation, projectName_repo);
		strcat(projectLocation, "/");
		strcat(projectLocation, newDirectory);
		res = mkdir(projectLocation,0777);
		
		printf("project loaction %s\n",projectLocation);		

		//create manifest for "./server_repos/<projectName>_repo/<projectName>"
		printf("%s\n", projectLocation);
		createManifest(projectLocation);
		write(sockfd, "complete", sizeof("complete"));
	}

	if (strcmp(buff, "checkout") == 0)  {
		getProj(sockfd, buff);
		printf("Trying to checkout file\n");
		
		//get "./server_repos/" path
		char server_repos[100];
		strcpy(server_repos, "server_repos/");
		
		//append "<projectName>_repo/" to "./server_repos/"
		char projectName_repo[100];
		strcpy(projectName_repo, server_repos);
		strcat(projectName_repo, buff);
		strcat(projectName_repo, "_repo");
		
		DIR *dir;
		dir = opendir(projectName_repo);
		
		char path[100];
		strcpy(path, projectName_repo);	
		strcat(path, "/");
		strcat(path, buff);
		
		printf("Path is %s\n", path);	
		//strcat(path, "/");
		//strcat(path, "payload.tar.gz");
		strcat(projectName_repo, "/");
		strcat(projectName_repo, "payload.tar.gz");

		compress(path, projectName_repo);
		
		printf("Tar is %s\n", projectName_repo);
		write(sockfd, "receive", sizeof("receive")); 
		sendItem(sockfd, projectName_repo); 		

		write(sockfd, "complete", sizeof("complete"));

	}

	
//----------------------------------------------------------------------------------
	if (strcmp(buff, "upgrade") == 0)  {
		getProj(sockfd, buff);
		printf("Trying to upgrade file\n");

		//get "./server_repos/" path
		char server_repos[100];
		strcpy(server_repos, "server_repos/");
		
		//append "<projectName>_repo/" to "./server_repos/"
		char projectName_repo[100];
		strcpy(projectName_repo, server_repos);
		strcat(projectName_repo, buff);
		strcat(projectName_repo, "_repo");
		
		DIR *dir;
		dir = opendir(projectName_repo);
		
		char path[100];
		strcpy(path, projectName_repo);	
		strcat(path, "/");
		strcat(path, buff);
		
		printf("Path is %s\n", path);	
		strcat(projectName_repo, "/");
		strcat(projectName_repo, "payload.tar.gz");

		compress(path, projectName_repo);
		
		printf("Tar is %s\n", projectName_repo);
		write(sockfd, "receive", sizeof("receive")); 
		sendItem(sockfd, projectName_repo); 		

		write(sockfd, "complete", sizeof("complete"));
	}
//----------------------------------------------------------------------------------
	if (strcmp(buff, "update") == 0)  {
		char clientProjectFolder[MAX];
		getProj(sockfd, clientProjectFolder);
		printf("Trying to update file\n\n");
		printf("client project to update: %s\n",clientProjectFolder);
		createUpdate(clientProjectFolder);
		write(sockfd, "complete", sizeof("complete"));
		}
	if (strcmp(buff, "destroy") == 0) {
		getProj(sockfd, buff);
		printf("Destroying repo\n");
		
		char server_repos[100];
		strcpy(server_repos, "server_repos/");
		
		//append "<projectName>_repo/" to "./server_repos/"
		char projectName_repo[100];
		strcpy(projectName_repo, server_repos);
		strcat(projectName_repo, "/");
		strcat(projectName_repo, buff);
		strcat(projectName_repo, "_repo");
		
		printf("Destroying %s\n", projectName_repo);	
		deleteDir(projectName_repo);
		printf("Deleted dir\n");
		write(sockfd, "complete", sizeof("complete"));	
	}

	if (strcmp(buff, "push") == 0)  {
		getProj(sockfd, buff);
		printf("Trying to push file\n");
		DIR *d;
		struct dirent *dir;
		d = opendir(buff);
		if (d) {
			write(sockfd, "send", strlen("send"));
			printf("Attempting to write to %s\n", buff);
			
			char repo[256];	
			strcpy(repo, "server_repos");
			strcat(repo, "/");
			strcat(repo, buff);
			strcat(repo, "_repo");
			
			char tarPath[256];
			strcpy(tarPath, repo);
			strcat(tarPath, "/");
			strcat(tarPath, "recv.tar.gz");
			
			char path[256];
			strcpy(path, repo);
			strcat(path, "/");
			strcat(path, buff);
			
			printf("Attempting to write to %s\n", tarPath);
			writeFile(sockfd, tarPath);
				
			char verPath[256];
			strcpy(verPath, repo);	
			strcat(verPath, "/");
			strcat(verPath, "1");
		
			printf("Path is %s\n", path);	
			printf("Taring to verPath %s\n", verPath);	
			compress(path, verPath);
			decompress(tarPath, path, 0);
			remove(tarPath);
			printf("File obtained, going to do stuff with it\n");
			printf("If project exists, tar it to it's version (if its ver 2, it becomes 2)\n");
			printf("Untar proj\n");
			printf("Update History\n");
		} else {
			printf("Cannot open directory\n");	
		}
	}
	if (strcmp(buff, "rollback") == 0) {
		getProj(sockfd, buff);
		printf("Trying to access proj %s\n", buff);
		char version[100];
		getVersion(sockfd, version);
		printf("Trying to get version %d\n", atoi(version));	
		
		//get "./server_repos/" path
		char server_repos[100];
		strcpy(server_repos, "server_repos/");
		
		//append "<projectName>_repo/" to "./server_repos/"
		char projectName_repo[100];
		strcpy(projectName_repo, server_repos);
		strcat(projectName_repo, buff);
		strcat(projectName_repo, "_repo");
		
		char projPath[100];
		strcpy(projPath, projectName_repo);
		strcat(projPath, "/");
		strcat(projPath, buff);	
		DIR *d;
		struct dirent *dir;
		d = opendir(projectName_repo);
		
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, buff) == 0) {
				printf("Deleting the project!\n");
				deleteDir(projPath);	
			}
			if (strcmp(dir->d_name, version) == 0) {
				printf("Going to untar version!\n");	
				printf("Dest is %s\n", projectName_repo);
				decompress(version, projectName_repo, 0);
			}
			if (atoi(dir->d_name) > atoi(version)) {
				printf("Should be deleting version %s\n", dir->d_name);
				char unlinkPath[100];
				strcpy(unlinkPath, projectName_repo);
				strcat(unlinkPath, "/");
				strcat(unlinkPath, dir->d_name);
				unlink(unlinkPath);	
			}
		}	
		write(sockfd, "complete", sizeof("complete"));
	}

	printf("Closing connection...\n");
	pthread_mutex_unlock(&mLock);
	close(sockfd);
	pthread_exit(NULL);
} 

int main(int argc, char** argv) 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("Failed to create socket.  Try again\n"); 
		exit(0); 
	} 
	else
		printf("Created socket\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	if (argc == 1) {
		printf("Server requires a port number, please enter a number between 1024 and 65535\n");
		exit(0);
	} else if (atoi(argv[1]) == 0) {
		printf("Please enter a valid port number\n");	
		exit(0);
	} else if (atoi(argv[1]) > 655535 || atoi(argv[1]) < 1024) {
		printf("Please enter a port in the range of 1024 and 65535\n");	
		exit(0);
	} 
	servaddr.sin_port = htons(atoi(argv[1])); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		printf("Failed to bind socket\n"); 
		exit(0); 
	} 
	else
		printf("Socket bound\n"); 

	//Listen on socket, set max connections to 20 (20+10) is MAX_BOUND
	if ((listen(sockfd, MAX_BOUND)) != 0) { 
		printf("Could not listen on socket\n"); 
		exit(0); 
	} 
	else
		printf("Server up!\n"); 
	len = sizeof(cli); 

	//Create threadPool of maximum 30 threads
	pthread_t threadPool[MAX_BOUND];
	int i = 0;
	while(1) {
		connfd = accept(sockfd, (struct sockaddr *)&cli, &len); 
		if (connfd < 0) { 
			printf("Accepting client...\n"); 
			exit(0); 
		}
		
		if(pthread_create(&threadPool[i], NULL, exec, &connfd) != 0 ) {
			printf("Thread creation failed\n");
		}	
		
		//If connections exceed 20 then we want to join all threads in our pool
		if( i >= 20)
		{
			i = 0;
			while(i < 20)
			{
				pthread_join(threadPool[i++],NULL);
			}
			i = 0;
		}

	} 
	return 0;
} 















