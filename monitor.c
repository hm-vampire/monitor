#include "httpd/httpd.h"
#include "httpd/log.h"
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

void* accept_request(void* arg)
{
	int fd = *(int*)arg;
	HttpPacket packet;
	memset(&packet, 0, sizeof(HttpPacket));
	packet.clientfd = fd;
	packet.conf.configpath = "/home/pi/home_monitor/conf";
	HTTPD_PacketHandler(&packet);
	HTTPD_CloseConnection(&packet);
}

int main()
{
	uint16_t port = 80;
	int fd = HTTPD_startup(port);
	if(-1 == fd)
		return -1;
	printf("Listen on port 80\n");
	
	int client_sock;
	struct sockaddr_in client_name;
	int client_name_len;
	
	pthread_t newthread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	while (1)
	{
		client_sock = accept(fd, (struct sockaddr *)&client_name, &client_name_len);
		if (client_sock == -1)
		{
			printf("accept error\n");
			return -1;
		}
		/* accept_request(client_sock); */
		if (pthread_create(&newthread , NULL, accept_request, (void*)&client_sock) != 0)
			perror("pthread_create");
	}
	
	pthread_attr_destroy(&attr);  
	HTTPD_Shutdown(fd);
	return 0;
}