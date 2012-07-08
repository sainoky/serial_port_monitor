#ifndef _SERIAL_UTIL_H
#define _SERIAL_UTIL_H

#ifndef _MSC_VER
#include <termios.h>

extern int serial_open(const char *fn, struct termios *tio_saved);
extern void serial_close(int fd, const struct termios *tio_saved);
#else	/* _MSC_VER */
#include <Windows.h>

extern HANDLE serial_open(const char *fn, DCB *dcb_saved);
extern void serial_close(HANDLE handle, const DCB *dcb_saved);
#endif	/* _MSC_VER */

#endif	/* _SERIAL_UTIL_H */
