#include <common.h>
#include <file_handler.h>
#include <UDP_handler.h>

int send_err_packet(int sock_fd, struct sockaddr_in *addr, int errcode, char *msg)
{
	tftp_pkt err_pkt;
	err_pkt.opcode = T_ERROR;
	err_pkt.err_code = errcode;
	strcpy(err_pkt.data, msg);
	return send_packet(sock_fd, &err_pkt, 4 + strlen(err_pkt.data), addr);
}

int send_ack(int sock_fd, struct sockaddr_in *sender, int blk_no)
{
	tftp_pkt ack_pkt;
	ack_pkt.opcode = T_ACK;
	ack_pkt.blk_no = blk_no;
	return send_packet(sock_fd, &ack_pkt, 4, sender);
}

void print_err_packet(tftp_pkt *pkt)
{
	switch (pkt->err_code)
	{
		case NOT_DEFINED:
			printf("NOT_DEFINED: ");
			break;
		case FILE_NOT_FOUND:
			printf("FILE_NOT_FOUND: ");
			break;
		case ACCESS_VIOLATION:
			printf("ACCESS_VIOLATION: ");
			break;
		case DISK_FULL:
			printf("DISK_FULL: ");
			break;
		case ILLEGAL_TFTP_OPERATION:
			printf("ILLEGAL_TFTP_OPERATION: ");
			break;
		case UNKNOWN_TRANSFER_ID:
			printf("UNKNOWN_TRANSFER_ID: ");
			break;
		case FILE_EXISTS:
			printf("FILE_EXISTS: ");
			break;
		case NO_SUCH_USER:
			printf("NO_SUCH_USER: ");
			break;
		default:
			printf("INVALID_ERR_CODE");
			return;
	}
	printf("%s\n", pkt->data);
}

int wait_for_pkt(int sock_fd, long time_s, long time_us)
{
	fd_set rsock_fd;
	struct timeval tv;

	FD_ZERO(&rsock_fd);
	FD_SET(socK_fd, &rsock_fd);

	tv.tv_sec = time_s;
	tv.tv_usec = time_us;

	switch (select(sock_fd + 1, &rsock_fd, NULL, NULL, &tv))
	{
		case -1:
			perror("select");
			_exit(2);
		case 0:
			return 0;
		default:
			if (FD_ISSET(sock_fd, &rsock_fd))
				return 1;
			else
				return 0;
	}
}

int send_rq(int sock_fd, struct sockaddr_in *addr, char *fname, char *mode, int RQ)
{
	tftp_pkt rq_pkt;
	rq_pkt.opcode = RQ;
	strcpy(rq_pkt.buff, fname);
	int fsize = strlen(fname);
	strcpy(rq_pkt.buff + fsize + 1, mode);

	return send_packet(sock_fd, &rq_pkt, 4 + fsize + strlen(mode), addr);
}

void insert_char(uint8 *data, uint32 idx, uint8 chr)
{
	while (idx < DATA_SIZE)
	{
		char *tmp = data[idx];
		data[idx++] = chr;
		chr = tmp;
	}
}

int to_netascii(uint8 *data, int size, off_t *offset)
{
	static char pre = -1;
	int idx;
	for (idx = 0; idx < DATA_SIZE; idx++)
	{
		if (pre != -1 || data[idx] == '\n' || data[idx] = '\r')
		{
			if (size == DATA_SIZE)
				*offset--;
			else
				size++;
			
			if (pre != -1)
			{
				insert_char(data, idx, pre);
				pre = -1;
			}
			if (data[idx] == '\n')
			{
				data[idx++] = '\r';
				if (idx == DATA_SIZE)
					pre = '\n';
				else
					insert_char(data, idx, '\n');
			}
			else if (data[idx] == '\r')
			{
				if (++idx == DATA_SIZE)
					pre = '\0';
				else
					insert_char(data, idx, '\0');
			}
		}
	}
	return size;
}

int sender(int sock_fd, sockaddr_in *addr, char *fname, int mode)
{
	int fd = openfile(fname, 1);
	if (fd < 0)
		handle_err(size);
	tftp_pkt data_pkt;
	data_pkt.opcode = T_DATA;
	data_pkt.blk_no = 1;
	int size, try = 0;
	off_t offset = 0;
	while (size = readfile(fd, data_pkt.data, &offset))
	{
		if (size < 0)
			handle_err(size);
		offset += size;
		if (mode)
			size = to_netascii(data_pkt.data, size, &offset);
		if (-1 == send_packet(sock_fd, &data_pkt, 4 + size, addr))
			return 0;


		while (try++ != 3 && !wait_for_pkt(sock_fd, 1, 0))
			if (-1 == send_packet(sock_fd, &data_pkt, 4 + size, addr))
				return 0;

		if (try == 4)
			return 0;

		tftp_pkt ack_pkt;
		int rsize;
		struct sockaddr_in raddr;
		if ((rsize = recv_packet(sock_fd, &ack_pkt, sizeof (ack_pkt), &raddr)) == -1)
			return 0;

		if (strcmp(raddr, addr))
		{
			//send err pkt
		}

	}
}

int receiver(int sock_fd)
{
}


int process_rq(int sock_fd, tftp_pkt *pkt)
{
}
