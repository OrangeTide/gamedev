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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "initgl.h"
#include "pixel.h"
#include "logger.h"
#include "sequence.h"
#include "lodepng.h"

static struct window *shell;
static struct sheet *main_sheet;
static int demo_x, demo_y;
static enum seq_id demo_id;
static struct seq_group **character_sg;
static unsigned character_sg_count;

enum {
	KEY_NONE = -1,
	KEY_ESC,
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	KEYCOUNT
};
static unsigned key[KEYCOUNT];
static char *keyname[KEYCOUNT] = {
	"Escape",
	"Up", "Down", "Left", "Right",
};

static struct sheet *
sheet_from_file(struct window *w, const char *filename)
{
	struct sheet *s;
	unsigned char *image;
	unsigned width, height;
	unsigned code;
	void *pixels;
	size_t rowbytes;
	unsigned x, y;

	code = lodepng_decode_file(&image, &width, &height, filename,
		LCT_RGBA, 8);
	if (code) {
		Error("%s:%s\n", filename, lodepng_error_text(code));
		return NULL;
	}

	s = sheet_new(shell, width, height);
	if (!s) {
		free(image);
		Error("%s:unable to create sheet\n", filename);
		return NULL;
	}
	pixels = sheet_pixels(s, &rowbytes);
	/* copy image in rgba32 format into a bgra32 format. */
	for (y = 0; y < height; y++) {
		void *row = getrow32(image, 0, y, 4 * width);

		for (x = 0; x < width; x++) {
			pixel_t c_in = getpixelfromrow32(row, x);
			pixel_t c_out;
			component_t r, g, b, a;

			pixel_get_rgba32(c_in, &r, &g, &b, &a);
			c_out = pixel_bgra32(r, g, b,a);
			putpixel32(pixels, x, y, rowbytes, c_out);
		}
	}
	sheet_sync(s);
	free(image);
	Info("%s:image %dx%d\n", filename, x, y);
	return s;
}

static struct sheet *
sheet_from_garbage(struct window *w)
{
	struct sheet *s;
	void *data;
	size_t rowbytes;
	int x, y;
	int width = 1024, height = 1024;

	s = sheet_new(shell, width, height);
	if (!s)
		return NULL;

	data = sheet_pixels(s, &rowbytes);
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			putpixel32(data, x, y, rowbytes, rand());
		}
	}
	sheet_sync(s);
}

static void
do_close(struct window *w, XEvent *ev)
{
	window_request_close(w);
}

static void
demo_sprite(struct window *w)
{
	int dst_x, dst_y, src_x, src_y, width, height;
	int s_width, s_height;
	int w_width, w_height;

	s_width = sheet_width(main_sheet);
	s_height = sheet_height(main_sheet);
	w_width = window_width(w);
	w_height = window_height(w);
	dst_x = rand() % w_width;
	dst_y = rand() % w_height;
	src_x = rand() % s_width;
	src_y = rand() % s_height;

	width = rand() % w_width;
	height = rand() % w_height;
	if (dst_x + width > w_width)
		width = w_width - dst_x;
	if (src_y + height > w_height)
		height = w_height - dst_y;
	if (src_x + width > s_width)
		width = s_width - src_x;
	if (src_y + height > s_height)
		height = s_height - src_y;
	window_draw_sheet(shell, dst_x, dst_y, main_sheet,
		src_x, src_y, width, height);
	window_flush(w);
}

static void
demo_update(struct window *w)
{
	int dst_x = demo_x * 8, dst_y = demo_y * 8;
	int ch = 0;
	const struct sequence *seq = character_sg[ch]->seq[demo_id];
	int step = 0;
	int src_x = seq->coord[step].x, src_y = seq->coord[step].y;
	int width = seq->width, height = seq->height;

#if 0 // excessive error checking
	int s_width, s_height;
	int w_width, w_height;

	s_width = sheet_width(main_sheet);
	s_height = sheet_height(main_sheet);
	w_width = window_width(w);
	w_height = window_height(w);

	if (dst_x + width > w_width)
		width = w_width - dst_x;
	if (src_y + height > w_height)
		height = w_height - dst_y;
	if (src_x + width > s_width)
		width = s_width - src_x;
	if (src_y + height > s_height)
		height = s_height - src_y;
#endif
	window_draw_sheet(shell, dst_x, dst_y, main_sheet,
		src_x, src_y, width, height);
	window_flush(w);
}

static void
do_key(void)
{
	if (event_parse_input_utf8(w, ev, buf, sizeof(buf))) {
		printf("input: \"%s\"\n", buf);
	}
}

static void
do_keyevent(int keycode)
{
	char buf[64];
	unsigned keycode;
	int down;

	if (event_parse_key(w, ev, &keycode, &down)) {
		printf("%s: %#x\n", down ? "press" : "release", keycode);
		if (keycode == key[KEY_UP]) {
			demo_y--;
			demo_id = SEQ_NORTH;
			demo_update(w);
		} else if (keycode == key[KEY_DOWN]) {
			demo_y++;
			demo_id = SEQ_SOUTH;
			demo_update(w);
		} else if (keycode == key[KEY_LEFT]) {
			demo_x--;
			demo_id = SEQ_WEST;
			demo_update(w);
		} else if (keycode == key[KEY_RIGHT]) {
			demo_x++;
			demo_id = SEQ_EAST;
			demo_update(w);
		} else if (keycode == key[KEY_ESC]) {
			window_request_close(w);
		}
	}
}

static void
do_paint(struct window *w, XEvent *ev)
{
	demo_update(w);
}

static void
key_init(void)
{
	unsigned i;

	for (i = 0; i < KEYCOUNT; i++) {
		int tmpkey = lookup_key(keyname[i]);
		if (tmpkey == -1) {
			Warning("could not load key \"%s\"\n", keyname[i]);
			key[i] = -1;
		} else {
			key[i] = tmpkey;
		}
	}
}

int
main()
{
	if (display_init() != INITGL_OK) {
		goto fail;
	}

	static struct window_callback_functions callbacks = {
		.paint = do_paint,
		// .reshape = do_reshape,
		.keyevent = do_keyevent,
		.typingkeyboard = do_inputevent,
		.exit = do_close,
	};

	if (window_new(&callbacks) != INITGL_OK) {
		goto fail;
	}

	my_key_init();
	gamepad_init();

	main_sheet = sheet_from_file(shell, "sheet1.png");
	if (!main_sheet) {
		abort(); // TODO: handle error
	}

	character_sg_count = 1;
	character_sg = calloc(character_sg_count, sizeof(*character_sg));
	if (!character_sg) {
		abort(); // TODO: handle errors
	}
	character_sg[0] = sequence_group_load("guy1.seq");
	if (!character_sg[0]) {
		abort(); // TODO: handle errors
	}

	if (my_gl_init() != OK) {
		goto fail;
	}

	while (!terminate_flag) {
		process_events();
		gamepad_poll();
		animate();
		paint_all();
	}

	window_paint(shell);
	display_wait(1, &shell);

	my_gl_done();
	gamepad_done();
	display_done();

	return 0;
fail:
	Error("FATAL ERROR!\n");
	window_free(shell);
	sheet_free(main_sheet);
	return 1;
}
