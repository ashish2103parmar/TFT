#include <common.h>
#include <UDP_handler.h>

int create_socket()
{
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1)
	{
		perror("socket");
		_exit(2);
	}
	return sock_fd;
}

int bind_socket(int sock_fd, char *ip_addr, unsigned short port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip_addr);
	addr.sin_port = htons(port);
	if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)))
	{
		perror("socket");
		return 0;
	}
	return 1;
}

int get_addr(struct sockaddr *addr, char **ip_addr, unsigned short *port)
{
	if (addr)
	{
		*ip_addr = inet_ntoa(((struct sockaddr_in *)addr)->sin_addr);
		*port = ntohs(((struct sockadd_in *)addr)->sin_port);
		return 1;
	}
	return 0;
}

int set_addr(struct sockaddr *addr, char *ip_addr, unsigned short port)
{
	if (ip_addr && port)
	{
		((struct sockaddr_in *)addr)->sin_family = AF_INET;
		((struct sockaddr_in *)addr)->sin_addr.s_addr = inet_addr(ip_addr);
		((struct sockaddr_in *)addr)->sin_port = htons(port);
		return 1;
	}
	return 0;
}

int recv_packet(int sock_fd, void *buff, unsigned int bsize, struct sockaddr *addr)
{
	int tmp, size = sizeof (struct sockaddr);
	if ((tmp = recvfrom(sock_fd, buff, bsize, 0, addr, &size)) == -1)
		perror("recv");
	return tmp;
}

int send_packet(int sock_fd, void *buff, unsigned int bsize, struct sockaddr *addr)
{
	int tmp;
	if (tmp = sendto(sock_fd, buff, bsize, 0, addr, sizeof (sockaddr)) == -1)
		perror("recv");
	return tmp;
}


