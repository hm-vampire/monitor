#ifndef UTILE_H
#define UTILE_H
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int readLine(int fd, char* buffer, size_t* linelen);
int isFileExist(char* filepath);
int getFileSize(char* filepath);
int getFileContent(char* filepath, char* fb);
int getFileSuffixLen(char* filename);
int getFileSuffix(char* filename, char* suffix);
#endif