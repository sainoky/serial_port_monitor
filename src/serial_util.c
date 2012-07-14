#include <stdio.h>
#ifndef _MSC_VER
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#define BAUDRATE B115200
#else	/* _MSC_VER */
#include <Windows.h>
#define BAUDRATE CBR_115200
#endif	/* _MSC_VER */

#ifndef _MSC_VER

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
#else	/* _MSC_VER */
static void serial_setup_dcb(HANDLE handle, const DCB *dcb_saved)
{
	DCB new_dcb;

	memcpy(&new_dcb, dcb_saved, sizeof(DCB));
	new_dcb.BaudRate = BAUDRATE;
	new_dcb.ByteSize = 8;
	new_dcb.Parity   = NOPARITY;
	new_dcb.StopBits = ONESTOPBIT;
	new_dcb.fDtrControl = TRUE;
	new_dcb.fRtsControl = TRUE;

	SetCommState(handle, &new_dcb);
}

HANDLE serial_open(const char *fn, DCB *dcb_saved)
{
	HANDLE handle;

	handle = CreateFile(fn, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			    OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (handle == INVALID_HANDLE_VALUE)
		return handle;

	GetCommState(handle, dcb_saved);
	serial_setup_dcb(handle, dcb_saved);
	return handle;
}

void serial_close(HANDLE handle, const DCB *dcb_saved)
{
	SetCommState(handle, (DCB *)dcb_saved);
	CloseHandle(handle);
}
#endif	/* _MSC_VER */
