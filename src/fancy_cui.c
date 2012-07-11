#include <stdio.h>
#ifndef _MSC_VER
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <termios.h>
#else	/* _MSC_VER */
#include <Windows.h>
#endif	/* _MSC_VER */
#include "serial_util.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifndef _MSC_VER
static void main_loop(int fd)
{
	struct termios newstdtio, oldstdtio;
	fd_set readfds;
	unsigned char c;
	ssize_t rsize;

	tcgetattr(STDIN_FILENO, &oldstdtio);
	memcpy(&newstdtio, &oldstdtio, sizeof(struct termios));
	newstdtio.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newstdtio);

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);
	FD_SET(fd, &readfds);

	for (;;) {
		fd_set rfds = readfds;
		int sel_result;

		sel_result = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);
		if (sel_result < 0) {
			perror("select");
			break;
		} else if (sel_result == 0) {
			/* timeout: no way */
		} else if (FD_ISSET(STDIN_FILENO, &rfds)) {
			rsize = read(STDIN_FILENO, &c, 1);
			if (rsize < 0) {
				perror("read");
				break;
			}
			printf("got data on stdin: %c\n", c);
			if (c == 'q')
				break;
			write(fd, &c, 1);
		} else if (FD_ISSET(fd, &rfds)) {
			rsize = read(fd, &c, 1);
			if (rsize < 0) {
				perror("read");
				break;
			}
			printf("\t\t\t\tgot data on serial: 0x%02x\n", c);
		}
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldstdtio);
}

#else	/* _MSC_VER */
static void _pwinerror(const char *msg, DWORD error_code)
{
	LPVOID lpMsgBuf;
	DWORD flags;

	flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;

	FormatMessage(flags, NULL, error_code,
		      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		      (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s: %s", msg, lpMsgBuf);

	LocalFree(lpMsgBuf);
}

static void pwinerror(const char *msg)
{
	_pwinerror(msg, GetLastError());
}

static int is_key_down_event(HANDLE stdin_hdl, unsigned char *c)
{
	INPUT_RECORD rec;
	DWORD len;

	if (!ReadConsoleInput(stdin_hdl, &rec, 1, &len)) {
		pwinerror("ReadConsoleInput");
		return 0;
	}
	if (rec.EventType != KEY_EVENT)
		return 0;
	if (!rec.Event.KeyEvent.bKeyDown)
		return 0;

	*c = rec.Event.KeyEvent.uChar.AsciiChar;
	return 1;
}

static int send_serial(HANDLE stdin_hdl, HANDLE serial_hdl)
{
	OVERLAPPED tmp_ov = {0};
	DWORD len;
	unsigned char c;

	if (!is_key_down_event(stdin_hdl, &c))
		return 0;

	printf("got data on stdin: %c\n", c);

	if (c == 'q')
		return 1;

	if (!WriteFile(serial_hdl, &c, sizeof(c), &len, &tmp_ov)) {
		DWORD e = GetLastError();
		if (e != ERROR_IO_PENDING) {
			_pwinerror("WriteFile", e);
			return 1;
		}
	}
	return 0;
}

int try_read(HANDLE serial_hdl, void *buf, int bytes, OVERLAPPED *ov)
{
	if (!ReadFile(serial_hdl, buf, bytes, NULL, ov)) {
		DWORD e = GetLastError();
		if (e != ERROR_IO_PENDING) {
			_pwinerror("ReadFile", e);
			return 1;
		}
	}
	return 0;
}

static void main_loop(HANDLE serial_hdl)
{
	HANDLE stdin_hdl;
	HANDLE hdls[2];
	OVERLAPPED ov = {0};
	unsigned char c;

	stdin_hdl = GetStdHandle(STD_INPUT_HANDLE);
	if (stdin_hdl == INVALID_HANDLE_VALUE) {
		pwinerror("GetStdHandle");
		return;
	}
	FlushConsoleInputBuffer(stdin_hdl);
	PurgeComm(serial_hdl,
		  PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (try_read(serial_hdl, &c, sizeof(c), &ov))
		return;

	/* When events occur at the same time, smaller index has the priority */
	hdls[0] = stdin_hdl;
	hdls[1] = ov.hEvent;

	for (;;) {
		int event;
		event = WaitForMultipleObjects(ARRAY_SIZE(hdls), hdls, FALSE,
					       INFINITE);

		if (event == WAIT_FAILED) {
			pwinerror("WaitForMultipleObjects");
			break;
		} else if (event == WAIT_OBJECT_0) {
			if (send_serial(stdin_hdl, serial_hdl))
				break;
		} else if (event == WAIT_OBJECT_0 + 1) {
			printf("\t\t\t\tgot data on serial: 0x%02x\n", c);
			if (try_read(serial_hdl, &c, sizeof(c), &ov))
				break;
		} else {
			printf("Unknown event: 0x%x\n", event);
			break;
		}
	}
	CloseHandle(ov.hEvent);
	CloseHandle(stdin_hdl);
}
#endif	/* _MSC_VER */

int main(int argc, char **argv)
{
#ifndef _MSC_VER
	int fd;
	struct termios oldtio;
#else	/* _MSC_VER */
	HANDLE handle;
	DCB olddcb;
#endif	/* _MSC_VER */
	char *devname;

	if (argc == 2) {
		devname = argv[1];
	} else {
		printf("usage: %s <serial device>\n", argv[0]);
		return 1;
	}

#ifndef _MSC_VER
	if ((fd = serial_open(devname, &oldtio)) < 0) {
		perror(devname);
#else	/* _MSC_VER */
	if ((handle = serial_open(devname, &olddcb)) < 0) {
		pwinerror(devname);
#endif	/* _MSC_VER */
		return 1;
	}

#ifndef _MSC_VER
	main_loop(fd);

	serial_close(fd, &oldtio);
#else	/* _MSC_VER */
	main_loop(handle);

	serial_close(handle, &olddcb);
#endif	/* _MSC_VER */

	return 0;
}
