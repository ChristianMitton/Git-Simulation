#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "deleteDir.h"

void deleteDir(char* path) {

	DIR* d = opendir(path);
	if (d) {
	struct dirent * dir;
	while ((dir = readdir(d)) != NULL) {
		if (strcmp(dir->d_name, ".") == 0) continue;
		if (strcmp(dir->d_name, "..") == 0) continue;
		char newPath[100];
		strcpy(newPath, path);
		strcat(newPath, "/");
		strcat(newPath, dir->d_name);
		printf("Newpath is %s\n", newPath);

		if (dir->d_type == DT_DIR) {
			char newPath[100];
			strcpy(newPath, path);
			strcat(newPath, "/");
			strcat(newPath, dir->d_name);
			printf("Newpath is %s\n", newPath);
			deleteDir(newPath);
			rmdir(newPath);
		}
		if (dir->d_type == DT_REG) { 
			unlink(newPath);
		}
	}
	closedir(d);
	rmdir(path);
	} else {
		printf("Nothing left to delete\n");
		return;	
	}
}

/*
//Driver for testing
int main(int argc, char** argv) {
	deleteDir("newProj");
	return 0;
}
*/
