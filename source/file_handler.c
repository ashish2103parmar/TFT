#include <common.h>
#include <file_handler.h>
#include <errno.h>

extern int errno;
#define file_mode 00644

int get_ferrtype()
{
	switch (errno)
	{
		case EACCES: 
			return -1 * ACCESS_VIOLATION - 1;
			break;
		case ENOENT:
			return -1 * FILE_NOT_FOUND - 1;
			break;
		case EDQUOT:
		case ENOSPC:
			return -1 * DISK_FULL - 1;
			break;
		case EEXIST:
			return -1 * FILE_EXISTS - 1;
			break;
		default:
			return -1 * NOT_DEFINED - 1;
			break;
	}
}

int openfile(char *fname, int rdonly)
{	
	int fd;
	if (rdonly)
		fd = open(fname, O_RDONLY);
	else
		fd = open(fname, O_WRONLY | O_CREAT | O_EXCL, file_mode);

	if (fd == -1)
	{
		fd = get_ferrtype();
		perror("open");
	}
	return fd;
}

int readfile(int fd, uint8 *data)
{
	int size = read(fd, (void *)data, DSIZE);
	if (size == -1)
	{
		size = get_ferrtype();
		perror("read");
	}
	return size;
}

int writefile(int fd, uint8 *data, uint32 dsize)
{
	int size = write(fd, (void *)data, dsize);
	if (size == -1)
	{
		size = get_ferrtype();
		perror("write");
	}
	return size;
}
