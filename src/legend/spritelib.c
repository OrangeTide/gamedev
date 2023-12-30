/*
 * Copyright (c) 2013 Jon Mayo <jon@rm-f.net>
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "spritelib.h"
#include "logger.h"

struct sheet;

struct sheet {
	struct sheet *next; /* next sheet on this window. */
	int width;
	int height;
};

/** Window API **/

/** Display API **/

void display_wait(int nr_windows, struct window **windows)
{
	XEvent ev;
	struct window *w;
	void (*f)(struct window *w, XEvent *ev);

	while (nr_windows > 0) {
		Debug("%s():nr_windows=%d\n", __func__, nr_windows);
		XFlush(display);

		XNextEvent(display, &ev);

		Debug("%s():%ld\n", __func__, ev.xany.window);
		w = window_find(ev.xany.window, nr_windows, windows);
		if (ev.xany.type < LASTEvent && w) {
			f = w->event[ev.xany.type];
			if (f)
				f(w, &ev);
		}
		window_cleanup(ev.xany.window, &nr_windows, windows);
	}
}

void display_init(void)
{
	int major, minor;

	display = XOpenDisplay(NULL);
	if (!display)
		exit(1);

	XInternAtoms(display, atom_names, ATOMCOUNT, False, atoms);

	if (!XRenderQueryExtension(display,
		&xrender_event_base, &xrender_error_base))
		goto not_present;

	if (!XRenderQueryVersion(display, &major, &minor))
		goto not_present;
	Info("XRENDER:version %d.%d\n", major, minor);
	return;
not_present:
	Error("XRENDER:extension not available\n");
	if (display)
		XCloseDisplay(display);
	exit(1);
}

/** Sheet API **/

/* depth=32 recommended */
static Visual *best_visual(int depth)
{
	XVisualInfo xvi;
	int screen = DefaultScreen(display);

	if (!XMatchVisualInfo(display, screen, depth, TrueColor, &xvi)) {
		Error("%s():no matching visual.\n", __func__);
		return NULL;
	}
	return xvi.visual;
}

struct sheet *sheet_new(struct window *window, unsigned width, unsigned height)
{
	struct sheet *s;
	Visual *visual;
	int depth = 32;
	XWindowAttributes attr;
	XImage *img;
	Pixmap pix;
	Window xwin;
	XRenderPictFormat *img_format;

	visual = best_visual(depth);
	if (!visual && window) {
		if (!XGetWindowAttributes(display, window->win, &attr)) {
no_visual:
			Error("%s():unable to find visual.\n", __func__);
			return NULL;
		}
		visual = attr.visual;
		depth = attr.depth;
	}
	if (!visual)
		goto no_visual;

	s = calloc(1, sizeof(*s));
	if (!s) {
		Error("%s():unable to create sheet.\n", __func__);
		return NULL;
	}
	s->width = width;
	s->height = height;

	if (!window) {
		Screen *screen = DefaultScreenOfDisplay(display);

		xwin = RootWindowOfScreen(screen);
	} else {
		xwin = window->win;
	}

	pix = XCreatePixmap(display, xwin, width, height, depth);
	if (!pix) {
		Error("%s():unable to create pixmap.\n", __func__);
		free(s);
		return NULL;
	}
	s->pix = pix;

	img_format = XRenderFindVisualFormat(display, visual);
	assert(img_format != NULL);
	if (!img_format) abort(); // TODO: implement error handler
	Debug("XRenderPictFormat:id=%#lx type=%d depth=%d cmap=%#lx\n",
		(long)img_format->id, img_format->type,
		img_format->depth, (long)img_format->colormap);
	Debug("Color format: red=%d/%#x green=%d/%#x blue=%d/%#x alpha=%d/%#x\n",
		img_format->direct.red, img_format->direct.redMask,
		img_format->direct.green, img_format->direct.greenMask,
		img_format->direct.blue, img_format->direct.blueMask,
		img_format->direct.alpha, img_format->direct.alphaMask);
	assert(img_format->depth == depth);
	s->pict = XRenderCreatePicture(display, pix, img_format, 0, NULL);
	if (!s->pict) {
		Error("%s():unable to create pict.\n", __func__);
		XFreePixmap(display, pix);
		free(s);
		return NULL;
	}

	s->mask = None; /* TODO: optionally support a mask */

	s->gc = XCreateGC(display, pix, 0, NULL);
	if (!s->gc) {
		Error("%s():unable to create gc.\n",
			__func__);
		XRenderFreePicture(display, s->pict);
		XFreePixmap(display, pix);
		free(s);
		return NULL;
	}

	img = XCreateImage(display, visual, depth, ZPixmap, 0, NULL,
		width, height, depth, 0);
	if (!img) {
		Error("%s():unable to create image.\n",
			__func__);
		XFreeGC(display, s->gc);
		XRenderFreePicture(display, s->pict);
		XFreePixmap(display, pix);
		free(s);
		return NULL;
	}
	img->data = calloc(img->bytes_per_line, height);
	if (!img->data) {
		Error("%s():unable to allocate image.\n",
			__func__);
		XDestroyImage(img);
		XRenderFreePicture(display, s->pict);
		XFreeGC(display, s->gc);
		XFreePixmap(display, pix);
		free(s);
		return NULL;
	}
	s->img = img;
	window_add_sheet(window, s);
	return s;
}

void sheet_free(struct sheet *s)
{
	if (!s)
		return;
	window_remove_sheet(s->window, s);
	// TODO: free up other stuff
	XDestroyImage(s->img);
	s->img = NULL;
	free(s);
}

unsigned sheet_width(const struct sheet *s)
{
	return s->img->width;
}

unsigned sheet_height(const struct sheet *s)
{
	return s->img->height;
}

void sheet_sync(const struct sheet *s)
{
	// TODO: finish this
	XPutImage(display, s->pix, s->gc, s->img, 0, 0, 0, 0,
		s->width, s->height);
}

void *sheet_pixels(struct sheet *s, size_t *rowbytes)
{
	XImage *img;

	if (!s || !rowbytes)
		return NULL;
	img = s->img;
	*rowbytes = img->bytes_per_line;
	return img->data;
}
