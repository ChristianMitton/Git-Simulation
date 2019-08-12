#ifndef CREATEUPDATE_H
#define CREATEUPDATE_H

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
/*
typedef struct Node{
	char *word;
	struct Node *next;
} Node;
*/
void createUpdateHelper(char *cManifest, char *sManifest, char *projectName);
void appendStringToUpdate(char *projectName, char *string);
void getDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]);
void createUpdate(char *projectName);

//void createUpdate(char *clientManifest, )
void createUpdate(char *projectName){

	char cManifestPath[100];
	strcpy(cManifestPath, "./");
	strcat(cManifestPath, projectName);
	strcat(cManifestPath, "/manifest.txt");

	//at this point, cManifestPath = "./<projectName>/manifest.txt";
	char project_repo[100];
	strcpy(project_repo, projectName);
	strcat(project_repo, "_repo/");

	char sManifestPath[100];
	strcpy(sManifestPath, "./server_repos/");
	
	strcat(sManifestPath, project_repo);
	strcat(sManifestPath, projectName);

	strcat(sManifestPath, "/manifest.txt");
	//at this point, sManifestPath = ./server_repos/<projectName>_repo/<projectName>/manifest.txt

	printf("cManifestPath: %s\n",cManifestPath); // ./<projectName>/manifest.txt"
	printf("sManifestPath: %s\n",sManifestPath); // ./server_repos/<projectName>_repo/<projectName>/manifest.txt

	//return;

	//put both paths into createUpdateHelper
	//printf("Creating update file...\n");	
	createUpdateHelper(cManifestPath, sManifestPath, projectName);
	//printf("Update file created\n");


	//update file has been created
	//print all contents of Update file to STDOUT
	char pathToUpdate[100];
	strcpy(pathToUpdate, "./");
	strcat(pathToUpdate, projectName);
	strcat(pathToUpdate, "/update.txt");
	FILE *fp = fopen(pathToUpdate, "r");
	if(fp == NULL){
		printf("can't open update\n");
		return;
	}
	char code[100]; int ver; char path[100]; char hash[100];
	bool fileIsEmpty = true;


	//printf("printing contents of newly update file..\n");

	while(fscanf(fp, "%s %d %s %s\n",&code,&ver,&path,&hash) != EOF){
		printf("%s %d %s %s\n",code,ver,path,hash);
		fileIsEmpty = false;
		/*
		if(strstr(path,"manifest.txt") != NULL){
			printf("setting fileIsEmpty to true\n");
			fileIsEmpty = true;
			continue;
		}
		printf("set fileIsEmpty to false\n");
		*/

	}
	if(fileIsEmpty == true){
		printf("Up to date\n");
	}


}


void createUpdateHelper(char *cManifest, char *sManifest, char *projectName){

	/*------------------------------------------------
	compare each line of client to every line in server
	--------------------------------------------------*/
	printf("_____cManifest>>>>> %s\n",cManifest);
	printf("_____sManifest>>>>> %s\n",sManifest);
	FILE *cfp = fopen(cManifest, "r");
	int cVer; char cPath[100]; char cHash[100];
	int clientManifestVer; char ctemp1[100]; char ctemp2[100];
	
	int count = 0;
	while(fscanf(cfp,"%d %s %s\n",&cVer,&cPath,&cHash) != EOF){
		
		if(strstr(cPath, "manifest.txt") != NULL){
			//printf("path contains 'manifest' continuing...\n");
			//continue;
		}
		if(count == 0){
			clientManifestVer = cVer;
			count++;
			continue;
		}
		FILE *sfp = fopen(sManifest, "r");
		int sVer; char sPath[100]; char sHash[100];
		int serverManifestVer; char stemp1[100]; char stemp2[100];

		int count2 = 0;
		printf("Client:\n");
		printf("- %d %s %s\n",cVer,cPath,cHash);
		bool pathMatch = false;
		//printf("sManifest: %s\n",sManifest);
		while(fscanf(sfp,"%d %s %s\n",&sVer,&sPath,&sHash) != EOF){
			if(strstr(sPath, "manifest.txt") != NULL){
				//continue;
			}
			if(count2 == 0){
				serverManifestVer = sVer;
				count2++;
				continue;
			}
			printf("%*s  Server:\n",3,"");
			printf("%*s - %d %s %s\n",6,"",sVer,sPath,sHash);
			
			if(strcmp(cPath,sPath) == 0){
				printf("%*s [---------MATCH--------]\n",6,"");
				pathMatch = true;
				//check if hash of file is not the same for both
				if(strcmp(cHash,sHash) == 1){
					if(sVer != cVer){
						appendStringToUpdate(projectName, "U ");
						char newVer[100];
						sprintf(newVer,"%d",sVer);
						appendStringToUpdate(projectName, newVer);
						appendStringToUpdate(projectName, " ");
						appendStringToUpdate(projectName, sPath);
						appendStringToUpdate(projectName, " ");
						appendStringToUpdate(projectName, sHash);
						appendStringToUpdate(projectName, "\n");
					}
				}
				if(serverManifestVer != clientManifestVer){
					appendStringToUpdate(projectName, "M ");
					char newVer[100];
					sprintf(newVer,"%d",sVer);
					appendStringToUpdate(projectName, newVer);
					appendStringToUpdate(projectName, " ");
					appendStringToUpdate(projectName, sPath);
					appendStringToUpdate(projectName, " ");
					appendStringToUpdate(projectName, sHash);
					appendStringToUpdate(projectName, "\n");

				}
			}
			//appendStringToUpdate();
		}
		
		if(pathMatch == false){
			if(clientManifestVer == serverManifestVer){
				printf("%*s [----------NO MATCH FOR (%s) and clientManiVer == serverManiVer----------]\n",6,"",cPath);
				appendStringToUpdate(projectName, "U ");
				appendStringToUpdate(projectName, "1 ");
				appendStringToUpdate(projectName, cPath);
				appendStringToUpdate(projectName, " ");
				appendStringToUpdate(projectName, cHash);
				appendStringToUpdate(projectName, "\n");
			}
			if(clientManifestVer != serverManifestVer){
				appendStringToUpdate(projectName, "D ");
				appendStringToUpdate(projectName, "1 ");
				appendStringToUpdate(projectName, cPath);
				appendStringToUpdate(projectName, " ");
				appendStringToUpdate(projectName, cHash);
				appendStringToUpdate(projectName, "\n");

			}
		}
		fclose(sfp);
	}

	fclose(cfp);

	/*------------------------------------------------
	compare each line of server to every line in client
	--------------------------------------------------*/
	FILE *sfp2 = fopen(sManifest, "r");
	int sVer2; char sPath2[100]; char sHash2[100];
	int serverManifestVer2; char stemp3[100]; char stemp4[100];


	int count3 = 0;
	while(fscanf(sfp2,"%d %s %s\n",&sVer2,&sPath2,&sHash2) != EOF){
		if(strstr(sPath2, "manifest.txt") != NULL){
			//continue;
		}
		if(count3 == 0){
			serverManifestVer2 = sVer2;
			count3++;
			continue;
		}
		FILE *cfp2 = fopen(cManifest, "r");
		int cVer2; char cPath2[100]; char cHash2[100];
		int clientManifestVer2; char ctemp3[100]; char ctemp4[100];
		int count4 = 0;
		printf("Server:\n");
		printf("- %d %s %s\n",sVer2,sPath2,sHash2);
		bool pathMatch = false;
		while(fscanf(cfp2,"%d %s %s\n",&cVer2,&cPath2,&cHash2) != EOF){
			if(strstr(cPath2, "manifest.txt") != NULL){
				continue;
			}
			if(count4 == 0){
				clientManifestVer2 = cVer2;
				count4++;
				continue;
			}
			printf("%*s  Client:\n",3,"");
			printf("%*s - %d %s %s\n",6,"",cVer2,cPath2,cHash2);
			if(strcmp(sPath2,cPath2) == 0){
				printf("%*s [---------MATCH--------]\n",6,"");
				pathMatch = true;
				//check if hash of file is not the same for both
				
			}
		}

		if(pathMatch == false){
			printf("%*s [----------NO MATCH FOR (%s)----------]\n",6,"",sPath2);
			
			if(serverManifestVer2 != clientManifestVer2){
				
				appendStringToUpdate(projectName, "A ");
				char newVer[100];
				sprintf(newVer,"%d",sVer2);
				appendStringToUpdate(projectName, newVer);
				appendStringToUpdate(projectName, sPath2);
				appendStringToUpdate(projectName, " ");
				appendStringToUpdate(projectName, sHash2);
				appendStringToUpdate(projectName, "\n");
			}
		}
	}

}

void appendStringToUpdate(char *projectName, char *string){
	
	char pathToProjUpdate[100];
	strcpy(pathToProjUpdate, "./");
	strcat(pathToProjUpdate, projectName);
	strcat(pathToProjUpdate, "/update.txt");
	printf("Writing to %s\n",pathToProjUpdate);
	int fd = open(pathToProjUpdate, O_RDWR | O_APPEND | O_CREAT, 0777);
	write(fd, string, strlen(string));
	close(fd);
	//printf("printed to %s\n",pathToProjUpdate);
}
/*
void getDirectoryPath(const char *name, int indent,char *targetDirectory, char finalPath[]){
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
		
		getDirectoryPath(path, indent + 2,targetDirectory,finalPath);
        } else {
*/		
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
/*		
	}
    }
    closedir(dir);
}
*/


#endif
