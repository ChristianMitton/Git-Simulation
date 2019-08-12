#ifndef UPGRADE_H
#define UPGRADE_H

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

void upgrade(char *projectName);


void upgrade(char *projectName){
	/*
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


	char updatePath[100];
	strcpy(updatePath, "./");
	strcat(updatePath, projectName);	
	strcat(updatePath, "/update.txt");

	FILE *cmfp = fopen(cManifestPath, "r");
	FILE *smfp = fopen(sManifestPath, "r");	
	FILE *cufp = fopen(updatePath, "r");

	if(smfp == NULL){
		return;
	}
	printf("printing contents of update\n");
	char code[100]; int ver; char path[100]; char hash[100];
	char mcode[100]; int ver; char path[100]; char hash[100];
	while(fscanf(cufp, "%s %d %s %s\n",&code,&ver,&path,&hash) != EOF){
		//while(fscanf(cmfp, "%s %d %s %s\n", ))
		printf("%s %d %s %s\n",code,ver,path,hash);
	}
	
	fclose(cmfp);
	fclose(smfp);
	fclose(cufp);
	*/
}




#endif
