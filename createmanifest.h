#ifndef CREATEMANIFEST_H
#define CREATEMANIFEST_H

#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
//#include "manifest.h"
//#include "generatehash.h"
#include <limits.h>
#include <string.h>

//Structs
typedef struct Node{
	char *word;
	struct Node *next;
} Node;

//Signatures
//Node *createNode(char *word);
Node *addToEnd(Node *head, Node *target);
void appendStringToManifest(char *pathToDirectory, char *string);
void listdir(const char *name, int indent, char *pathToDirectory);
char* getHash(char *filename);
void getDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]);
void createManifest(char *directoryName);

void createManifest(char *directoryName){
	//traverse through directory
	char directoryPath[100]; getDirectoryPath(".", 0,directoryName, directoryPath);
	strcpy(directoryPath, directoryName);
	printf("Directory path: %s",directoryPath);
	//CONTINUE FROM HERE
	appendStringToManifest(directoryPath, "1 ");
	appendStringToManifest(directoryPath, directoryName);
	appendStringToManifest(directoryPath, " ");
	appendStringToManifest(directoryPath, "Manifest\n");
	listdir(directoryName, 0, directoryPath);
	//log each path in manifest		
	
}

void getDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]){
    DIR *dir;
    struct dirent *entry;

    if(!(dir = opendir(name))){
        return;
	}

    while ((entry = readdir(dir)) != NULL) {
	char path[1024];
        if(entry->d_type == DT_DIR) {
		//char path[1024];
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			continue;
		}
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);

		char pathToDirectory[100];
		char slash[100];
		strcpy(pathToDirectory, name);
		strcpy(slash, "/");
		
		strcat(pathToDirectory, slash);
		strcat(pathToDirectory, entry->d_name);


		if(strcmp(entry->d_name,targetDirectory) == 0){
			printf("%s == %s\n",entry->d_name, targetDirectory);
			strcpy(finalPath,pathToDirectory); 
			strcat(finalPath,slash);
		}
		//printf("%s\n", entry->d_name);
		//printf("%*s[%s]\n", indent, "", entry->d_name);
		//printf("%*sPATH:[%s]\n", indent, "", path);
		//printf("%s\n",pathToDirectory);
		
		getDirectoryPath(path, indent + 2,targetDirectory,finalPath);
        } 
    }
    closedir(dir);
}


void listdir(const char *name, int indent, char *pathToDirectory){
    DIR *dir;
    struct dirent *entry;

    if(!(dir = opendir(name))){
        return;
	}
    char actualPath[PATH_MAX + 1];
    while ((entry = readdir(dir)) != NULL) {
	char path[1024];
        if(entry->d_type == DT_DIR) {
		//char path[1024];
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			continue;
		}
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
		//printf("%s\n", entry->d_name);
		printf("%*s[%s]\n", indent, "", entry->d_name);
		//printf("%*sPATH:[%s]\n", indent, "", path);
		
		listdir(path, indent + 2, pathToDirectory);
        } else {
		
		//realpath(entry->d_name, actualPath);
		char pathToFile[100];
		char slash[100];
		strcpy(pathToFile, name);
		strcpy(slash, "/");
		
		strcat(pathToFile, slash);
		strcat(pathToFile, entry->d_name);
		//NEW HASH:
		char *hash = getHash(pathToFile);
		//ORIGINAL HASH: 
		//char *hash = getHash(entry->d_name);

		// in hash should be name + "/entry->d_name"

		printf("%*s- %s\n",indent,"",entry->d_name);
		//printf("%*s  actualPath: %s\n", indent,"", actualPath);
		printf("%*s  File hash: %s\n",indent, "",hash);
		//printf("%*s  path: %s\n",indent,"",path);	
		//printf("%*s  name: %s\n",indent,"",name);	
		printf("%*s  pathToFile: %s\n",indent,"",pathToFile);
		
		appendStringToManifest(pathToDirectory, "1 ");
		appendStringToManifest(pathToDirectory, pathToFile);
		appendStringToManifest(pathToDirectory, " ");
		if(hash != NULL){
			appendStringToManifest(pathToDirectory, hash);
		} else {
			appendStringToManifest(pathToDirectory, "(null)");
		}
		appendStringToManifest(pathToDirectory, "\n");
		
	}
    }
    closedir(dir);
}

void appendStringToManifest(char *pathToDirectory, char *string){
	char manifest[100];
	strcpy(manifest, pathToDirectory);
	strcat(manifest,"/manifest.txt");
	int fd = open(manifest, O_RDWR | O_APPEND | O_CREAT, 0777);
	write(fd, string, strlen(string));
	close(fd);
}
/*
Node *createNode(char *word){
	Node *ptr = malloc(sizeof(Node));
	char word1[50];
	char slash[50];

	strcpy(word1, word);
	strcpy(slash, "/");

	strcat(slash, word1);
		
	ptr->word = strdup(slash);
	return ptr;
}

Node *addToEnd(Node *head, Node *target){
	if(head == NULL){
		return target;
	}
	Node *temp = createNode(target->word);
	Node *ptr = head;
	while(ptr->next != NULL){
		ptr = ptr->next;
	}
	ptr->next = temp;
	return head;
}
*/

char* getHash(char *filename){
//int main(int argc, char **argv){
	//attempt to open file
	int fd, bytes, filesize;
	char* buffer;
	struct stat mystat;
	if((fd = open(filename, O_RDONLY)) == -1){
		//fprintf(stderr, "Incorrect number of arguments\n");
		return 0;
	}
	if(stat(filename, &mystat) == -1){
		//fprintf(stderr, "Incorrect number of arguments\n");
		return 0;
	}	
	filesize = (int) mystat.st_size;
	if((buffer = (char*)malloc(filesize*sizeof(char))) == NULL ){
		//fprintf(stderr, "Incorrect number of arguments\n");
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
	//printf("Final Hash:\n%s\n",output);
	char *finalHash = output;
	return finalHash;
}


#endif
