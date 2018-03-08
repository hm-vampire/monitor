#include "utile.h"

int readLine(int fd, char* buffer, size_t* linelen)
{
	if(0==fd || NULL == buffer || NULL == linelen)
		return -1;
	int index = 0;
	int ret = 0;
	char temp;
	do
	{
		ret = read(fd, &temp, 1);
		if(-1 == ret)
			return -1;
		else if(0 == ret)
			return 0;
		else if(temp == '\r')
		{
			buffer[index++] = temp;
			ret = read(fd, &temp, 1);
			if(-1 == ret)
				return -1;
			else if(0 == ret)
				return 0;
			else if(temp == '\n')
			{
				buffer[index++] = temp;
				*linelen = index;
				break;
			}
		}
		buffer[index++] = temp;
	}while(1);
	return ret;
}

int isFileExist(char* filepath)
{
	if(NULL == filepath)
		return -1;
	int ret = access(filepath, R_OK);
	return ret;
}

int getFileSize(char* filepath)
{
	if(isFileExist(filepath) == -1)
		return -1;
	struct stat attr;
	int ret = stat(filepath, &attr);
	if(-1 == ret )
		return -1;
	return attr.st_size;
}

int getFileContent(char* filepath, char* fb)
{
	if(NULL == fb || isFileExist(filepath) == -1)
		return -1;
	int fd = open(filepath, 0);
	if(-1 == fd)
		return -1;
	size_t filesize = getFileSize(filepath);
	int ret = read(fd, fb, filesize);
	close(fd);
	return ret;
}

int getFileSuffixLen(char* filename)
{
	if(NULL == filename)
		return -1;
	size_t filenamelen = strlen(filename);
	int suffixlen = 0;
	
	if(!strchr(filename, '.'))
		return 0;
	
	while(filenamelen)
	{
		if(filename[--filenamelen] == '.')
		{
			return suffixlen;
		}
		suffixlen++;
	}
	return -1;
}

int getFileSuffix(char* filename, char* suffix)
{
	if(NULL == filename || NULL == suffix)
		return -1;
	size_t suffixlen = getFileSuffixLen(filename);
	if(0 == suffixlen) return 0;
	if(-1 == suffixlen) return -1;
	
	size_t prefixlen = strlen(filename) - suffixlen;
	strncpy(suffix, filename+prefixlen, suffixlen);
	return suffixlen;
}