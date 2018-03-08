#include "httpd.h"
#include "log.h"
#include "utile.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

static char* ContentType[] = {"text/plain", "text/xml", "text/html", "image/jpeg", "image/png", "image/gif", "video/mpeg4", "video/avi", "audio/wav", "audio/mp3"};
int HTTPD_MimeHandler(char* filename)
{
	if(NULL == filename)
		return -1;

	char* suffix = NULL;
	size_t suffixlen = getFileSuffixLen(filename);
	if(-1 == suffixlen) return -1;
	else if(0 == suffixlen) return 0;
	else
	{
		suffix = (char*)malloc(suffixlen+1);
		if(NULL == suffix) return -1;
		memset(suffix, 0, suffixlen+1);
		if(-1 == getFileSuffix(filename, suffix))
			return -1;
	}
	if(!strcmp(suffix, "sor") || !strcmp(suffix, "sol") || !strcmp(suffix, "txt"))
		return 0;
	else if(!strcmp(suffix, "vxml") || !strcmp(suffix, "xml") || !strcmp(suffix, "xpl"))
		return 1;
	else if(!strcmp(suffix, "htm") || !strcmp(suffix, "html") || !strcmp(suffix, "htx") || !strcmp(suffix, "jsp") || !strcmp(suffix, "xhtml"))
		return 2;
	else if(!strcmp(suffix, "jfif") || !strcmp(suffix, "jpe") || !strcmp(suffix, "jpeg") || !strcmp(suffix, "jpg"))
		return 3;
	else if(!strcmp(suffix, "png"))
		return 4;
	else if(!strcmp(suffix, "gif"))
		return 5;
	else if(!strcmp(suffix, "m4e") || !strcmp(suffix, "mp4"))
		return 6;
	else if(!strcmp(suffix, "avi"))
		return 7;
	else if(!strcmp(suffix, "wav"))
		return 8;
	else if(!strcmp(suffix, "mp3"))
		return 9;
	else
	{
		
	}
	if(suffix) free(suffix);
	return -1;
}

int HTTPD_startup(uint16_t port)
{
	int httpd = 0;
	struct sockaddr_in name;

	httpd = socket(AF_INET, SOCK_STREAM, 0);
	if (httpd == -1)
	{
		printf("socket error\n");
		return -1;
	}
	
	int optval = 1;  
	if (setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {  
		printf("setsockopt error");  
		return -1;  
	} 
  
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (-1 == bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
	{
		printf("bind error\n");
		return -1;
	}
	if (listen(httpd, 5) < 0)
	{
		printf("listen error\n");
		return -1;
	}
	return httpd;	
}

void HTTPD_Shutdown(int fd)
{
	close(fd);
}

void HTTPD_CloseConnection(HttpPacket* packet)
{
	if(NULL == packet)
		return;
	close(packet->clientfd);
	if(packet->uri) free(packet->uri);
	if(packet->filepath) free(packet->filepath);
	if(packet->data) free(packet->data);
}

int HTTPD_PacketHandler(HttpPacket* packet)
{
	if(NULL == packet)
		return -1;
	char line[10000];
	size_t linelen;
	
	memset(line, 0, 10000);
	if(-1 == readLine(packet->clientfd, line, &linelen))
		return -1;
	
	HTTPD_GetUriMethod(packet, line, linelen);
	printf("method[%s], uri:[%s]\n", packet->method, packet->uri);
	
	do
	{
		memset(line, 0, 10000);
		readLine(packet->clientfd, line, &linelen);
		//printf("%d %s", linelen, line);
	}while(strncmp(line, "\r\n", 2));
	
	if(!strcmp(packet->method, "GET"))
	{
		HTTPD_GetHandler(packet);
	}
	else if(!strcmp(packet->method, "HEAD"))
	{
		HTTPD_Unimplement(packet);
	}
	else if(!strcmp(packet->method, "POST"))
	{
		HTTPD_Unimplement(packet);
	}
	else if(!strcmp(packet->method, "PUT"))
	{
		HTTPD_Unimplement(packet);
	}
	else if(!strcmp(packet->method, "DELETE"))
	{
		HTTPD_Unimplement(packet);
	}
	else if(!strcmp(packet->method, "CONNECT"))
	{
		HTTPD_Unimplement(packet);
	}
	else if(!strcmp(packet->method, "OPTIONS"))
	{
		HTTPD_Unimplement(packet);
	}
	else if(!strcmp(packet->method, "TRACE"))
	{
		HTTPD_Unimplement(packet);
	}
	return 0;
}

int HTTPD_GetHandler(HttpPacket* packet)
{
	if(NULL == packet)
		return -1;
	char* uri =  packet->uri;
	size_t urilen = strlen(uri);
	char filepath[10000];
	memset(filepath, 0, 10000);
	
	if(!strcmp(uri, "/monitorpic"))
	{
		packet->messagetype = MONITORPIC;
		return HTTPD_Unimplement(packet);
	}
	else if(!strcmp(uri, "/monitorstream"))
	{
		packet->messagetype = MONITORSTREAM;
		return HTTPD_Unimplement(packet);
	}
	else
	{
		sprintf(filepath, "%s", packet->conf.configpath);
		if(urilen == 1 && !strcmp(uri, "/"))
		{
			strcat(filepath, "/index.html");
		}
		else
		{
			strcat(filepath, uri);
		}
		
		int contenttype = HTTPD_MimeHandler(filepath);
		if(-1 == contenttype)
			return -1;
		else if(0 <= contenttype && 5 >= contenttype)
		{
			packet->messagetype = TP;
		}
		else if(6 <= contenttype && 7 >= contenttype)
		{
			packet->status = NOTIMPLEMENT;
			packet->messagetype = STREAM;
			return HTTPD_Unimplement(packet);
		}
		else if(8 <= contenttype && 9 >= contenttype)
		{
			packet->status = NOTIMPLEMENT;
			packet->messagetype = AUDIO;
			return HTTPD_Unimplement(packet);
		}
		else
		{
			packet->status = NOTIMPLEMENT;
			return HTTPD_Unimplement(packet);
		}

		if(-1 == isFileExist(filepath))
		{
			packet->status = INTERNALERROR;
			return HTTPD_InternalError(packet);
		}
		
		size_t filesize = getFileSize(filepath);
		packet->data = (char*)malloc(filesize);
		if(NULL == packet->data) return HTTPD_InternalError(packet);
		getFileContent(filepath, packet->data);
		packet->datalen = filesize;

		packet->status = OK;
		packet->connection = 0;
		packet->maxage = 0;
		packet->expires = 0;
		packet->cachecontrol = 0;
		packet->contenttype = contenttype;
		packet->contentlength = filesize;		
	}
	return HTTPD_ResponseHandler(packet);
}

int HTTPD_ResponseHandler(HttpPacket* packet)
{
	if(NULL == packet)
		return -1;
	char resp[100000];
	memset(resp, 0, 100000);
	if(packet->messagetype == MONITORPIC)
	{
		
	}
	else if(packet->messagetype == MONITORSTREAM)
	{
		
	}
	else if(packet->messagetype == TP)
	{
		sprintf(resp, "HTTP/1.1 200 OK\r\n");
		strcat(resp, "Server: vampire\r\n");
		strcat(resp, "Connection: close\r\n");
		strcat(resp, "Max-Age: 0\r\n");
		strcat(resp, "Expires: 0\r\n");
		strcat(resp, "Cache-Control: no-cache, private\r\n");
		strcat(resp, "Pragma: no-cache\r\n");
		
		char contenttype[1000];
		memset(contenttype, 0, 1000);
		sprintf(contenttype, "ContentType: %s\r\n", ContentType[packet->contenttype]);
		strcat(resp, contenttype);
		
		char contentlength[1000];
		memset(contentlength, 0, 1000);
		sprintf(contentlength, "ContentLength: %d\r\n", packet->contentlength);
		strcat(resp, contentlength);
		strcat(resp, "\r\n");
	}
	else if(packet->messagetype == AUDIO)
	{
		
	}
	else if(packet->messagetype == STREAM)
	{
		
	}
	write(packet->clientfd, resp, strlen(resp));
	write(packet->clientfd, packet->data, packet->datalen);	
	return 0;
}

int HTTPD_GetUriMethod(HttpPacket* packet, char* line, size_t linelen)
{
	if(NULL == packet || NULL == line)
		return -1;
	int idx = 0;
	char* position[2] = {NULL, NULL};
	int blankcount = 0;
	while(idx<linelen)
	{
		if(line[idx] == ' ')
		{
			position[blankcount] = &line[idx];
			blankcount++;
			if(2 == blankcount) break;
		}
		idx++;
	}
	int methodlen = position[0]-line;
	strncpy(packet->method, line, methodlen);
	
	int urilen = position[1] - position[0]-1;
	packet->uri = (char*)malloc(urilen+1);
	if(NULL == packet->uri) return -1;
	memset(packet->uri, 0, urilen+1);
	strncpy(packet->uri, line+ methodlen + 1, urilen);
	printf("methodlen: %d, urilen: %d\n", methodlen, urilen);
	return 1;
}

int HTTPD_Unimplement(HttpPacket* packet)
{
	char resp[100000];
	memset(resp, 0, 100000);
	packet->status = NOTIMPLEMENT;
	strcpy(resp, "HTTP/1.1 501 Not Implemented\r\n");
	strcat(resp, "Server: vampire\r\n");
	strcat(resp, "Content-Type: text/html\r\n");
	
	size_t filesize = getFileSize("/home/pi/home_monitor/conf/notimplement.html");
	if(-1 == filesize)
		return -1;
	
	char temp[1000];
	memset(temp, 0, 1000);
	sprintf(temp, "Content-Length: %d\r\n", filesize);
	strcat(resp, temp);
	strcat(resp, "\r\n");
	
	char* fb = (char*)malloc(filesize);
	if(NULL == fb)
		return -1;
	int ret = getFileContent("/home/pi/home_monitor/conf/notimplement.html", fb);
	if(-1 == ret)
		return -1;
	
	write(packet->clientfd, resp, strlen(resp));
	write(packet->clientfd, fb, filesize);
	if(fb) free(fb);
	return 0;
}

int HTTPD_InternalError(HttpPacket* packet)
{
	char resp[100000];
	memset(resp, 0, 100000);
	packet->status = NOTIMPLEMENT;
	strcpy(resp, "HTTP/1.1 500 Internal Server Error\r\n");
	strcat(resp, "Server: vampire\r\n");
	strcat(resp, "Content-Type: text/html\r\n");
	
	size_t filesize = getFileSize("/home/pi/home_monitor/conf/internalerror.html");
	if(-1 == filesize)
		return -1;
	
	char temp[1000];
	memset(temp, 0, 1000);
	sprintf(temp, "Content-Length: %d\r\n", filesize);
	strcat(resp, temp);
	strcat(resp, "\r\n");
	
	char* fb = (char*)malloc(filesize);
	if(NULL == fb)
		return -1;
	int ret = getFileContent("/home/pi/home_monitor/conf/internalerror.html", fb);
	if(-1 == ret)
		return -1;
	
	write(packet->clientfd, resp, strlen(resp));
	write(packet->clientfd, fb, filesize);
	if(fb) free(fb);
	return 0;
}

