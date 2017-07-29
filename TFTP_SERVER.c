#include <common.h>
#include <file_handler.h>
#include <UDP_handler.h>
#include <TFTP_handler.h>


#define IPADDR "192.168.43.150"
#define TFTP_PORT 6900

int main()
{
	unsigned short port = 20000;
	int sock_fd = create_socket();
	bind_socket(sock_fd, IPADDR, TFTP_PORT);
	tftp_pkt buff;
	chdir("shared/");
	printf("Server started at %s:%d\n", IPADDR, TFTP_PORT);
	struct sockaddr_in addr;
	while (1)
	{
		if (0 < recv_packet(sock_fd, &buff, DATA_SIZE + 4, &addr))
		{
			switch (fork())
			{
				case -1:
					perror("fork");
					_exit(1);
				case 0:
					close(sock_fd);
					sock_fd = create_socket();
					bind_socket(sock_fd, IPADDR, port++);
					if (!process_rq(sock_fd, &addr, &buff))
						printf("failed\n");
					_exit(5);
			}
		}
	}
}
