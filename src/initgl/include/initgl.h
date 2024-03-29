#ifndef INITGL_H_
#define INITGL_H_
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include "log.h"

#define INITGL_OK (0)
#define INITGL_ERR (-1)

#define initgl_gl_check() do { \
		GLenum e = glGetError(); \
		if (e != GL_NO_ERROR) {\
			log_error("%s:%d:%s():GL error = 0x%04x\n", __FILE__, __LINE__, __func__, e); \
			abort(); \
		} \
	} while(0)

extern int terminate_flag;

typedef void window_paintfunc(void);
typedef void window_idlefunc(void);
typedef void window_reshapefunc(int width, int height);
typedef void window_keyeventfunc(int key);
typedef void window_typingkeyboardfunc(const char *s, unsigned len);

struct window_callback_functions {
	window_paintfunc *paint;
	window_idlefunc *idle;
	window_reshapefunc *reshape;
	window_keyeventfunc *keyevent;
	window_typingkeyboardfunc *typingkeyboard;
};

void display_done(void);
int display_init(void);
void paint_all(void);
int window_new(const struct window_callback_functions *callbacks);
void process_events(void);
int lookup_key(const char *s);
unsigned long long clock_now(void);
double clock_diff(unsigned long long t1, unsigned long long t0);

void window_register_paint(window_paintfunc *f);
void window_register_idle(window_idlefunc *f);
void window_register_reshape(window_reshapefunc *f);
void window_register_keyevent(window_keyeventfunc *f);
void window_register_typingkeyboard(window_typingkeyboardfunc *f);
#endif
