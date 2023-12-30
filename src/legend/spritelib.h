/*
 * Copyright (c) 2013,2023 Jon Mayo <jon@rm-f.net>
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

#ifndef SPRITELIB_H
#define SPRITELIB_H

struct sheet;
struct window;

void window_free(struct window *w);
struct window *window_new(int width, int height);
void window_request_close(struct window *w);
void window_flush(struct window *w);
void window_paint(struct window *w);
int window_width(const struct window *w);
int window_height(const struct window *w);
void *window_get_ptr(struct window *w);
void window_set_ptr(struct window *w, void *ptr);
void window_event(struct window *w, int type,
	void (*f)(struct window *w, XEvent *ev));
void window_draw_box(struct window *w, int x, int y, int width, int height,
	XRenderColor color);
void window_draw_sheet(struct window *w, int dst_x, int dst_y, struct sheet *s,
	int src_x, int src_y, int width, int height);

int event_parse_input_utf8(struct window *w, XEvent *ev,
	char *buf, size_t bufmax);
int event_parse_key(struct window *w, XEvent *ev, unsigned *keycode, int *down);

int keycode_from_name(const char *name, unsigned *keycode);

void display_wait(int nr_windows, struct window **windows);
void display_init(void);

struct sheet *sheet_new(struct window *window, unsigned width, unsigned height);
void sheet_free(struct sheet *s);
unsigned sheet_width(const struct sheet *s);
unsigned sheet_height(const struct sheet *s);
void sheet_sync(const struct sheet *s); // TODO: rename sync_flush
void *sheet_pixels(struct sheet *s, size_t *rowbytes);

#endif
