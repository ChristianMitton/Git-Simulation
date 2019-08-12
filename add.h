#ifndef ADD_H
#define ADD_H

#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
//#include "generatehash.h"
#include <limits.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

void getPath(const char *name, int indent,char *targetFile, char finalPath[]);
void getDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]);
char* getHash(char *filename);
void copyFileToDirectory(char *fileToBeCopied, char *directoryToCopyInto);
//void appendStringToManifest(char *string);
void appendStringToManifestAdd(char *pathToProjectManifest, char *string);
void add(char *directoryName, char *file);


void add(char *directoryName, char *file){
	//Add is done on client side, so directory path will always be in folder "./<project>"
	char directoryPath[100]; 
	//getDirectoryPath(".",0,directoryName,directoryPath);
	strcpy(directoryPath, "./");
	strcat(directoryPath, directoryName);
	strcat(directoryPath, "/");
	printf("Directory Path: %s\n",directoryPath);
	//return;
	copyFileToDirectory(file, directoryPath);
	
	//return 0;

	//printf("file is: %s\n",file);
	//search for given file in manifest
	//char *filePath = ""; getPath(".",0,file,&filePath);

	//the new file needs to be added to the project directory

	//char filePath[100]; getPath(directoryName,0,file,filePath);
	char filePath[100]; getPath(".",0,file,filePath);
	char *fileHash = getHash(filePath);	
	//printf("file path is: %s\n",filePath);	
	
	char pathToProjectManifest[100];
	char m[100]; 
	strcpy(m,"manifest.txt");
	strcat(pathToProjectManifest, directoryPath);
	strcat(pathToProjectManifest, m);
	printf("pathToProjectManifest> : %s\n",pathToProjectManifest);

	FILE *fp = fopen(pathToProjectManifest,"r");
	int currVer; char currPath[100]; char currHash[100];
	bool alreadyExsists = false;
	while(fscanf(fp, "%d %s %s\n",&currVer,&currPath,&currHash) != EOF){
		printf("comparing %s and %s\n", filePath, currPath);
		if(strcmp(filePath,currPath) == 0){
			printf("%s == %s\n", filePath, currPath);
			alreadyExsists = true;
		}
	}
	if(alreadyExsists == false){
		
		appendStringToManifestAdd(pathToProjectManifest, "1 ");
		appendStringToManifestAdd(pathToProjectManifest, filePath);
		appendStringToManifestAdd(pathToProjectManifest, " ");
		appendStringToManifestAdd(pathToProjectManifest, fileHash);
		appendStringToManifestAdd(pathToProjectManifest, "\n");
		
	}
	
	fclose(fp);	
	
}

void appendStringToManifestAdd(char *pathToProjectManifest, char *string){
	int fd = open(pathToProjectManifest, O_RDWR | O_APPEND | O_CREAT, 0777);
	printf("opened %s\n",pathToProjectManifest);
	write(fd, string, strlen(string));
	close(fd);
}

void getPath(const char *name, int indent,char *targetFile, char finalPath[]){
    DIR *dir;
    struct dirent *entry;

    if(!(dir = opendir(name))){
        return;
	}
 //   char actualPath[PATH_MAX + 1];
    while ((entry = readdir(dir)) != NULL) {
	char path[1024];
        if(entry->d_type == DT_DIR) {
		//char path[1024];
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			continue;
		}
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
		//printf("%s\n", entry->d_name);
		//printf("%*s[%s]\n", indent, "", entry->d_name);
		//printf("%*sPATH:[%s]\n", indent, "", path);
		
		getPath(path, indent + 2,targetFile,finalPath);
        } else {
		
		//realpath(entry->d_name, actualPath);
		char pathToFile[100];
		char slash[100];
		strcpy(pathToFile, name);
		strcpy(slash, "/");
		
		strcat(pathToFile, slash);
		strcat(pathToFile, entry->d_name);
	
		if(strcmp(targetFile,entry->d_name)==0){
			//*finalPath = pathToFile; 
			strcpy(finalPath,pathToFile); 
			//printf("%s\n",finalPath);
		}

			
		//printf("%*s  pathToFile: %s\n",indent,"",pathToFile);
		
	}
    }
    closedir(dir);
}


void copyFileToDirectory(char *fileToBeCopied, char *directoryPath){

	printf("fileToBeCopied: %s\n",fileToBeCopied);	
	int fd = open(fileToBeCopied, O_RDONLY | O_APPEND | O_CREAT, 0777);

	if(fd < 0){
		printf("fd error\n");
		return;
	}
	char fileAddedToDirectoryPath[100];
	strcpy(fileAddedToDirectoryPath, directoryPath);
	strcat(fileAddedToDirectoryPath, fileToBeCopied);

	//   fileAddedToDirectoryPath = ./././filename.txt

	int fd2 = open(fileAddedToDirectoryPath, O_RDWR | O_APPEND | O_CREAT, 0777);
	if(fd2 < 0){
		printf("fd2 error\n");
		return;
	}
	//char *directoryPath = getDirectoryPath(directory);
	char c;
	while(read(fd, &c, 1) != 0){
		//go into directory, and re write filename
		printf("c: %c\n",c);
		write(fd2, &c, 1);			
		//pritnf("path to directory: %s\n");
		//int fd2 = open();
	}
	close(fd);
	close(fd2);


}



#endif
