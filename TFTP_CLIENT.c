#include <common.h>
#include <file_handler.h>
#include <UDP_handler.h>
#include <TFTP_handler.h>


#define IPADDR "192.168.43.202"
#define TFTP_PORT 6900

int main()
{
	int sock_fd = create_socket();
	tftp_pkt buff;
	chdir("shared1/");
	struct sockaddr_in addr, addr1;
	int idx;
	set_addr(&addr, IPADDR, TFTP_PORT);
	char buff[100], *arg, mode[10];
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
		}
		if (!strcmp(buff, "quit"))
		{
			return 0;
		}
		else if (!strcmp(buff, "put"))
		{
			send_rq(sock_fd, &addr, "TFTP_handler.c", "octet", T_WRQ);
			while (!wait_for_ack(sock_fd, &addr1, 0));
			{
			unsigned int size = sender(sock_fd, "TFTP_handler.c", &addr1, 0);
			if (size > 0)
				printf("File sent %s\n", arg);
			else
				printf("error\n");
			}
			break;
		}
		else if (!strcmp(buff, "get"))
		{
		}
		else if (!strcmp(buff, "connect"))
		{
		}
		else if (!strcmp(buff, "mode"))
		{
			if (!strcmp(mode, ))
		}
		else 
			printf("Invalid command\n");
	}
}
