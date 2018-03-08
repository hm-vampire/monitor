#ifndef HTTPD_H
#define HTTPD_H
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum statuscode{OK = 1, NOTIMPLEMENT, INTERNALERROR}STATUSCODE;
typedef enum messagetype{UNKNOW = 0, MONITORPIC = 1, MONITORSTREAM, TP, AUDIO, STREAM}MESSAGETYPE;

typedef struct conf
{
	char* configpath;
}CONF;

typedef struct string
{
	char* data;
	size_t datalen;
}String;

typedef struct httppacket
{
	CONF conf;
	char* uri;
	char method[10];
	MESSAGETYPE messagetype;
	
	STATUSCODE status;
	char server[100];
	int connection;
	int maxage;
	int expires;
	int cachecontrol;
	uint8_t contenttype;
	size_t contentlength;
	
	int clientfd;
	char* filepath;
	char* data;
	size_t datalen;
	
}HttpPacket;

int HTTPD_startup(uint16_t port);
void HTTPD_Shutdown(int fd);
void HTTPD_CloseConnection(HttpPacket* packet);
int HTTPD_PacketHandler(HttpPacket* packet);
int HTTPD_ResponseHandler(HttpPacket* packet);
int HTTPD_MimeHandler(char* filename);
int HTTPD_GetUriMethod(HttpPacket* packet, char* line, size_t linelen);
int HTTPD_Unimplement(HttpPacket* packet);
int HTTPD_InternalError(HttpPacket* packet);
int HTTPD_GetHandler(HttpPacket* packet);
#endif