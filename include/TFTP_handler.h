
#ifndef TFTP_HANDLER
#define TFTP_HANDLER


int send_err_packet(int sock_fd, struct sockaddr_in *addr, int errcode, char *msg);
int send_ack(int sock_fd, struct sockaddr_in *sender, int blk_no);
void print_err_packet(tftp_pkt *pkt);
int wait_for_pkt(int sock_fd, long time_s, long time_us);
int wait_for_ack(int sock_fd, struct sockaddr_in *addr, int blk_no);
int send_rq(int sock_fd, struct sockaddr_in *addr, char *fname, char *mode, int RQ);
void insert_char(uint8 *data, uint32 idx, uint8 chr);
int to_netascii(uint8 *data, int size, off_t *offset);
int sender(int sock_fd, char *fname, struct sockaddr_in *addr, int mode);
//unsigned int receiver(int sock_fd, char *fname, struct sockaddr_in *addr, int mode);
unsigned int receiver(int sock_fd, int fd, struct sockaddr_in *addr, int mode);
int process_rq(int sock_fd, struct sockaddr_in *addr, tftp_pkt *pkt);
	
#endif
