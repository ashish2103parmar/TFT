#include <common.h>
#include <file_handler.h>
#include <UDP_handler.h>
#include <TFTP_handler.h>


#define IPADDR "127.0.0.1"
#define TFTP_PORT 6900

int main(int argc, char *argv[])
{
	int sock_fd = create_socket();
	tftp_pkt buff;
	chdir("shared1/");
	struct sockaddr_in addr, addr1;
	set_addr(&addr, IPADDR, TFTP_PORT);
	//char buff[100];
	while (1)
	{
		//printf("TFTP>");
		//fflush(stdout);
		//scanf(" %[^\n]", buff);
		send_rq(sock_fd, &addr, "TFTP_handler.c", "octet", T_WRQ);
		while (!wait_for_ack(sock_fd, &addr1, 0));
		{
		unsigned int size = sender(sock_fd, "TFTP_handler.c", &addr1, 0);
		if (size > 0)
			printf("sent\n");
		else
			printf("error\n");
		}
		break;
	}
}
