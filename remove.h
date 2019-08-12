#ifndef REMOVE_H
#define REMOVE_H

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

void rGetDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]);
void removeFileFromDirectoryPath(char *directoryPath, char *targetFile);
void appendStringToManifestCopy(char *directoryPath, char *string);
void removeFile(char *directoryName, char *file);

//int main(int argc, char **argv){
void removeFile(char *directoryName, char *file){
	/*	
	if(argc != 3){
		printf("Please provide a directory and a file to remove\n");
		return 0;
	}

	char *directoryName = argv[1];
	char *file = argv[2];
	*/

	char directoryPath[100];
	rGetDirectoryPath(".",0,directoryName, directoryPath);
	printf("Directory Path: %s\n",directoryPath);

	removeFileFromDirectoryPath(directoryPath, file);

	//----------------remove file from manifest
	char pathToProjectManifest[100];
	char m[100];
	strcpy(m,"manifest.txt");
	strcpy(pathToProjectManifest, directoryPath);
	
	strcat(pathToProjectManifest, m);
	printf("pathToProjectManifest: %s\n",pathToProjectManifest);
//---------------------------------
	//search manifest for filename, keep track of line number that contains file to be removed
	FILE *fp = fopen(pathToProjectManifest,"r");

	int currVer; char currPath[100]; char currHash[100];

	int count = 1;
	while(fscanf(fp, "%d %s %s\n",&currVer,&currPath,&currHash) != EOF){
		if(strstr(currPath, file) != NULL){
			printf("File removed is at line %d\n",count);
			continue;
		}

		char verAsString[100];
		sprintf(verAsString, "%d", currVer);
		appendStringToManifestCopy(directoryPath, verAsString);
		appendStringToManifestCopy(directoryPath, " ");
		appendStringToManifestCopy(directoryPath, currPath);
		appendStringToManifestCopy(directoryPath, " ");
		appendStringToManifestCopy(directoryPath, currHash);
		appendStringToManifestCopy(directoryPath, "\n");
		count++;
	}

	
	char manifestPath[100];
	strcpy(manifestPath,directoryPath);
	strcat(manifestPath, "manifest.txt");
	printf("path to manifest is: %s\n",manifestPath);
	remove(manifestPath);

	//rename manifestCopy.txt to manifest.txt
	char newManifestPath[100];
	strcpy(newManifestPath,directoryPath);
	strcat(newManifestPath, "manifestCopy.txt");
	printf("path to manifestCopy is: %s\n",newManifestPath);	
	rename(newManifestPath, manifestPath);
	
}

void appendStringToManifestCopy(char *directoryPath, char *string){
	char manifestCopyPath[100];
	strcpy(manifestCopyPath,directoryPath);
	strcat(manifestCopyPath, "manifestCopy.txt");
	int fd = open(manifestCopyPath, O_RDWR | O_APPEND | O_CREAT, 0777);
	printf("writing to %s\n",manifestCopyPath);

	write(fd, string, strlen(string));
	close(fd);
}


void rGetDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]){
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
		
		rGetDirectoryPath(path, indent + 2,targetDirectory,finalPath);
        } else {
		
		//realpath(entry->d_name, actualPath);
		/*
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
		*/

			
		//printf("%*s  pathToFile: %s\n",indent,"",pathToFile);
		
	}
    }
    closedir(dir);
}


void removeFileFromDirectoryPath(char *directoryPath, char *targetFile){
	char filePath[100];
	strcpy(filePath, directoryPath);
	strcat(filePath, targetFile);
	
	printf("Path to file: %s\n",filePath);
	remove(filePath);
	
}


#endif
