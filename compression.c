#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "compression.h"
void decompress(char* targetName, char* parentDir, int isRoot) {
	DIR *d;
	d = opendir(parentDir);
	char buff[512];
	struct dirent *dir;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strstr(dir->d_name, targetName) == NULL) continue;
			if (isRoot) {
				if (snprintf(buff, sizeof(buff), "tar xvf %s --strip-components=2 -C .", dir->d_name) >= sizeof(buff)) {
					printf("Error decompressing, beyond buffer memory\n");
					return;	
				}
			} else if (isRoot == 0) {
				char full[100];
				strcpy(full, parentDir);
				strcat(full, "/");
				strcat(full, targetName);

				if (snprintf(buff, sizeof(buff), "tar xvf %s", full) >= sizeof(buff)) {
					printf("Error decompressing, beyond buffer memory\n");
					return;	
				}
			}

			fflush(NULL);
			int exeCmd = system(buff);
			if (exeCmd > 0) {
				printf("Error decompressing, cannot execute command\n");	
				return;
			}
		}
		close(d);
	}
	
}


void compress(char* target, char* parent) {
	char buff[512];
	if (snprintf(buff, sizeof(buff), "tar cvf %s %s", parent, target) >= sizeof(buff)) {
		printf("Error compressing, beyond buffer memory\n");
		return;	
	}
	printf("Buff is %s\n", buff);
	fflush(NULL);
	int exeCmd = system(buff);
	if (exeCmd > 0) {
		printf("Error compressing, cannot execute command\n");	
		return;
	}
}


