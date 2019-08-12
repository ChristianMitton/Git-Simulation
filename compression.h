#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#ifndef COMPRESSION_H
#define COMPRESSION_H

void decompress(char* targetName, char* parent, int isRoot);
void compress(char* target, char* parent);

#endif
