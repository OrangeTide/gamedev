#include "initgl.h"
#include "log.h"

/**********************************************************************/

static int up_key, down_key, left_key, right_key, fire_key;

/**********************************************************************/

static void
my_gl_init(void)
{
	// TODO: load state
}

static void
my_key_init(void)
{
	up_key = lookup_key("Up");
	down_key = lookup_key("Down");
	left_key = lookup_key("Left");
	right_key = lookup_key("Right");
	fire_key = lookup_key("z");

	log_debug("up=%d down=%d left=%d right=%d fire=%d", up_key, down_key, left_key, right_key, fire_key);
}

static void
paint(void)
{
	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	// TODO: scale and center the input to fill the output
	// in_x_ofs = 0;
	// in_y_ofs = 0;
	// in_width = width * scale;
	// in_height = height * scale;
}

static void
keyevent(int key)
{
	if (key == up_key) {
		log_debug("Up");
	} else if (key == down_key) {
		log_debug("Down");
	} else if (key == left_key) {
		log_debug("Left");
	} else if (key == right_key) {
		log_debug("Right");
	} else if (key == fire_key) {
		log_debug("Fire!");
	}
}

static void
inputevent(const char *s, unsigned len)
{
	log_debug("Typing Event: \"%.*s\"", len, s);
}

static struct window_callback_functions callbacks = {
	.paint = paint,
	.reshape = reshape,
	.keyevent = keyevent,
	.typingkeyboard = inputevent,
};

int
main()
{
	if (display_init() != INITGL_OK) {
		return 1;
	}
	if (window_new(&callbacks) != INITGL_OK) {
		return 1;
	}
	my_key_init();
	my_gl_init();

	while (!terminate_flag) {
		process_events();
		paint_all();
	}

	display_done();

	return 0;
}
