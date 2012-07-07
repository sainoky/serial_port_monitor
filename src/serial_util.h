#ifndef _SERIAL_UTIL_H
#define _SERIAL_UTIL_H

#include <termios.h>

extern int serial_open(const char *fn, struct termios *tio_saved);
extern void serial_close(int fd, const struct termios *tio_saved);

#endif	/* _SERIAL_UTIL_H */
