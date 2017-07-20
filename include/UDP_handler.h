
#ifndef UDP_HANDLER
#define UDP_HANDLER 0

int create_socket();
int bind_socket(int sock_fd, char *ip_addr, unsigned short port);
int get_addr(struct sockaddr_in *addr, char **ip_addr, unsigned short *port);
int set_addr(struct sockaddr_in *addr, char *ip_addr, unsigned short port);
int recv_packet(int sock_fd, void *buff, unsigned int bsize, struct sockaddr_in *addr);
int send_packet(int sock_fd, void *buff, unsigned int bsize, struct sockaddr_in *addr);

#endif
