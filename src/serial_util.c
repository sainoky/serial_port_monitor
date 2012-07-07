#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define BAUDRATE B115200

static void serial_setup_tio(int fd)
{
	struct termios newtio;

	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
}

int serial_open(const char *fn, struct termios *tio_saved)
{
	int fd;

	fd = open(fn, O_RDWR | O_NOCTTY);
	if (fd < 0)
		return fd;

	tcgetattr(fd, tio_saved);
	serial_setup_tio(fd);
	return fd;
}

void serial_close(int fd, const struct termios *tio_saved)
{
	tcsetattr(fd, TCSANOW, tio_saved);
	close(fd);
}
