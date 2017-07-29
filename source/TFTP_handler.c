#include <common.h>
#include <file_handler.h>
#include <ctype.h>
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
	FD_SET(sock_fd, &rsock_fd);

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

int wait_for_ack(int sock_fd, struct sockaddr_in *addr, int blk_no)
{
	if (wait_for_pkt(sock_fd, 1, 0) == 1)
	{
		tftp_pkt *pkt = malloc(sizeof (tftp_pkt));
		int pkt_size;
		if ((pkt_size = recv_packet(sock_fd, (void *)pkt, sizeof (tftp_pkt), addr)) > 0)
		{
			if (pkt->opcode == T_ACK && pkt->blk_no == blk_no)
			{
				free(pkt);
				return 1;
			}
			else if (pkt->opcode == T_ERROR)
			{
				print_err_packet(pkt);
			}
		}		
	}
	return 0;
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
		char tmp = data[idx];
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
		if (pre != -1 || data[idx] == '\n' || data[idx] == '\r')
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


int sender(int sock_fd, char *fname, struct sockaddr_in *addr, int mode)
{
	int fd = openfile(fname, 1);
	off_t offset = 0;
	int bytes;
	if (fd < 0)
	{
		send_err_packet(sock_fd, addr, fd * -1 - 1, "Couldn't open file\r\n");
		return 0;
	}
	tftp_pkt pkt;
	pkt.opcode = T_DATA;
	pkt.blk_no = 1;
	while (1)
	{
		int bytes = readfile(fd, pkt.data, &offset);
		if (bytes < 0)
		{
			send_err_packet(sock_fd, addr, bytes * -1 - 1, "Send Data Failed\r\n");
			return 0;
		}
		offset = offset + bytes;
		if (mode)
			bytes = to_netascii(pkt.data, bytes, &offset);

		if (0 > send_packet(sock_fd, &pkt, 4 + bytes, addr))
			return 0;
		else
			printf("Sending block %d\t\t\t\t", pkt.blk_no);
		int idx;
		for (idx = 0; idx < 3; idx++)
		{
			if (wait_for_ack(sock_fd, addr, pkt.blk_no))
			{
				printf("ACK %d received\n", pkt.blk_no);
				break;
			}
			else
				if (0 > send_packet(sock_fd, &pkt, 4 + bytes, addr))
					return 0;
				else
					printf("Timed out\nResending block %d\t\t\t\t", pkt.blk_no);

		}
		if (idx == 3)
			return 0;
		if (bytes < 512)
		{
			close(fd);
			return 1;
		}
		pkt.blk_no++;
	}
	return 0;
}

unsigned int receiver(int sock_fd, int fd, struct sockaddr_in *addr, int mode)
{
	if (fd < 0)
	{
		send_err_packet(sock_fd, addr, fd * -1 - 1, "Couldn't open file\r\n");
		return 0;
	}
	tftp_pkt pkt;
	int bytes, blk_no = 1;
	unsigned int size = 0;
	while (1)
	{
		if (wait_for_pkt(sock_fd, 5, 0))
		{
			bytes = recv_packet(sock_fd, &pkt, sizeof (pkt), addr);
			if (bytes == -1)
				return 0;
			if (pkt.opcode == T_DATA)
			{
				if (pkt.blk_no == blk_no)
				{
					if (mode)
					{
						bytes = write_netascii(fd, pkt.data, bytes - 4);
					}
					else
					{
						bytes = writefile(fd, pkt.data, bytes - 4);
					}
					if (bytes < 0)
					{
						send_err_packet(sock_fd, addr, bytes * -1 - 1, "Send Data Failed\r\n");
						return 0;
					}
					size += bytes;
					printf("Received block %d\t\t\t\t", blk_no);

					send_ack(sock_fd, addr, blk_no);
					printf("ACK %d sent\n", blk_no++);
					if (bytes < 512)
						break;
				}
				else if (pkt.blk_no == blk_no - 1 && blk_no - 1)
				{
					send_ack(sock_fd, addr, blk_no - 1);
					printf("resending ACK %d\n", blk_no - 1);
				}
				else
				{
					send_err_packet(sock_fd, addr, ILLEGAL_TFTP_OPERATION, "invalid cmd\r\n");
					if (blk_no)
						return 0;
				}
			}
			else if (pkt.opcode == T_ERROR)
			{
				print_err_packet(&pkt);
				return 0;
			}
			else
			{
				send_err_packet(sock_fd, addr, ILLEGAL_TFTP_OPERATION, "invalid cmd\r\n");
				return 0;
			}
		}
		else
			return 0;
	}
	close(fd);
	return size;
}

int process_rq(int sock_fd, struct sockaddr_in *addr, tftp_pkt *pkt)
{
		
	int idx;
	if (pkt->opcode == T_RRQ)
	{
		char *fname = pkt->buff;
		char *mode = pkt->buff + strlen(fname) + 1;
		/*while (mode[idx] != '\0') 
		{
			mode[idx] = toupper(mode[idx]);
			idx++;
		}*/
		return sender(sock_fd, fname, addr, !strcmp(mode, "NETASCII"));
	}
	else if (pkt->opcode == T_WRQ)
	{
		char *fname = pkt->buff;
		char *mode = pkt->buff + strlen(fname) + 1;
		/*while (mode[idx] != '\0') 
		{
			mode[idx] = toupper(mode[idx]);
			idx++;
		}*/
		send_ack(sock_fd, addr, 0);
		int fd = openfile(fname, 0);
		unsigned int size = receiver(sock_fd, fd, addr, !strcmp(mode, "NETASCII"));
		if (size)
			printf("wrote %d bytes to file %s\n", size, fname);
		else
			return 0;
	}
	else
	{
		send_err_packet(sock_fd, addr, ILLEGAL_TFTP_OPERATION, "invalid request\r\n");
		return 0;
	}
	return 1;
}
