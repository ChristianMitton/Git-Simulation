#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


//Needs to be compiled like gcc hashing.c -lcrypto

int main(int argc, char **argv){
	//take file as argument
	if(argc != 2){
		fprintf(stderr, "Incorrect number of arguments\n");
		return 0;
	}
	//attempt to open file
	int fd, bytes, filesize;
	char* buffer;
	struct stat mystat;
	if((fd = open(argv[1], O_RDONLY)) == -1){
		fprintf(stderr, "Incorrect number of arguments\n");
		return 0;
	}
	if(stat(argv[1], &mystat) == -1){
		fprintf(stderr, "Incorrect number of arguments\n");
		return 0;
	}	
	filesize = (int) mystat.st_size;
	if((buffer = (char*)malloc(filesize*sizeof(char))) == NULL ){
		fprintf(stderr, "Incorrect number of arguments\n");
		return 0;	
	}
	//Set up SHA
	SHA256_CTX sha256;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	char output[65];
	SHA256_Init(&sha256);
	while(bytes = read(fd, buffer, filesize)){
		SHA256_Update(&sha256, buffer, bytes);
	}
	SHA256_Final(hash, &sha256);
	int i;
	for(i = 0; i < SHA256_DIGEST_LENGTH; i++){
		sprintf(output+(i*2), "%02x", hash[i]);
	}
	output[64] = '\0';
	printf("Final Hash:\n%s\n",output);
	return 0;
}

