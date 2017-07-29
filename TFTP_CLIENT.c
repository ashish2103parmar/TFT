#include <common.h>
#include <file_handler.h>
#include <UDP_handler.h>
#include <TFTP_handler.h>


#define IPADDR "127.0.0.1"
#define TFTP_PORT 6900
#define TRY_LIMIT 3

int main()
{
	int sock_fd = create_socket();
	chdir("shared1/");
	struct sockaddr_in addr, addr1;
	int idx;
	set_addr(&addr, IPADDR, TFTP_PORT);
	char buff[100], *arg, mode[10];
	strcpy(mode, "octect");
	unsigned int size;
	while (1)
	{
		printf("TFTP>");
		fflush(stdout);
		scanf(" %[^\n]", buff);
		idx = 0;
		while (buff[idx])
		{
			if (buff[idx] == ' ')
			{
				buff[idx] = '\0';
				arg = buff + idx + 1;
				break;
			}
			idx++;
		}
		if (!strcmp(buff, "bye"))
		{
			return 0;
		}
		else if (!strcmp(buff, "put"))
		{
			send_rq(sock_fd, &addr, arg, mode, T_WRQ);
			for (idx = 0; idx < TRY_LIMIT; idx++) 
			{
				if (wait_for_ack(sock_fd, &addr1, 0))
				{
					size = sender(sock_fd, arg, &addr1, !strcmp(mode, "netascii"));
					if (size > 0)
						printf("File sent %s\n", arg);
					else
						printf("Transfer failed\n");
					break;
				}
				else
				{	
					printf("Failed... trying again\n");
					send_rq(sock_fd, &addr, arg, mode, T_WRQ);
				}
			}
		}
		else if (!strcmp(buff, "get"))
		{
			int fd = open(arg, O_WRONLY | O_CREAT);
			if (fd < 0)
				printf("coundnt open/create file\n");
			else
			{
				send_rq(sock_fd, &addr, arg, mode, T_RRQ);
				size = receiver(sock_fd, fd, &addr1, !strcmp(mode, "netascii"));
				if (size > 0)
					printf("File %s received\n%u bytes received\n", arg, size);
				else
					printf("Transfer failed\n");
			}
		}
		else if (!strcmp(buff, "connect"))
		{
			if (set_addr(&addr, arg, TFTP_PORT))
				printf("IP address %s set\n", arg);
			else
				printf("invalid address\n");
		}
		else if (!strcmp(buff, "mode"))
		{
			if (!strcmp(arg, "netascii") || !strcmp(arg, "octect"))
			{
				strcpy(mode, arg);	
				printf("%s mode selected\n", arg);
			}
			else
				printf("invalid mode\n");
		}
		else 
			printf("Invalid command\n");
		*buff = '\0';
		arg = NULL;
	}
}
