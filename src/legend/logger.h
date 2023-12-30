/*
 * Copyright (c) 2012-2013 Jon Mayo
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* a complicated set of macros to support empty __VA_ARGS__ macros.
 * example:
 * Debug("Check\n");
 * Debug("Check %d\n", 12);
 * Debug("Check %d %s %d\n", 42, "Hello", 99);
 *
 * Info() and Error() macros don't use __func__ and __LINE__
 *
 * Limited to 64 arguments before breaking macro evaluation.
 */
#ifndef DEBUG_H
#define DEBUG_H
# include <stdio.h>

#define CONCAT(x, y) CONCAT_(x, y)
#define CONCAT_(x, y) x##y

#define ID(...) __VA_ARGS__

#define IFMULTIARG(if,then,else) \
	CONCAT(IFMULTIARG_, IFMULTIARG_(if, \
	Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, \
	Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, \
	Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, \
	Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, \
	Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, \
	Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, \
	Y, Y, N, ))(then,else)
#define IFMULTIARG_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, \
	_10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
	_20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
	_30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
	_40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
	_50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
	_60, _61, _62, _63, ...) _63
#define IFMULTIARG_N(then, else) else
#define IFMULTIARG_Y(then, else) then

#define PROVIDE_SECOND_ARGUMENT(x, y, ...) \
	CONCAT(IFMULTIARG(ID(__VA_ARGS__), INSERT_, ADD_), \
	SECOND_ARGUMENT) (x, y, __VA_ARGS__)

#define ADD_SECOND_ARGUMENT(x, y, z) z, x, y
#define INSERT_SECOND_ARGUMENT(x, y, z, ...) z, x, y, __VA_ARGS__

#include <string.h>
#include <errno.h>

#ifdef USE_SYSLOG
# include <syslog.h>
# define Warning(...) syslog(LOG_WARNING, __VA_ARGS__)
# define Error(...) syslog(LOG_ERR, __VA_ARGS__)
# define Info(...) syslog(LOG_INFO,  __VA_ARGS__)
# define SysError() syslog(LOG_ERR, "Error:%s():%d:%s\n", \
	__func__, __LINE__, strerror(errno));
#else
# define Warning(...) fprintf(stderr, "Warning:" __VA_ARGS__)
# define Error(...) fprintf(stderr, "Error:" __VA_ARGS__)
# define Info(...) fprintf(stderr, "Info:"  __VA_ARGS__)
# define SysError() fprintf(stderr, "Error:%s():%d:%s\n", \
	__func__, __LINE__, strerror(errno));
#endif

/* don't support Debug() output to syslog */
#ifdef NDEBUG
# define Debug(...) do { } while(0)
#else
# define Debug(...) fprintf(stderr, "Debug:%s():%d:" \
	PROVIDE_SECOND_ARGUMENT(__func__, __LINE__, __VA_ARGS__))
#endif

#endif
