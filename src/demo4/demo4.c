#include "initgl.h"
#include "log.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define USE_GLES2 1

#define OK (0)
#define ERR (-1)

/**********************************************************************/

static int up_key, down_key, left_key, right_key, fire_key;

static GLint attr_vertex;
static GLint attr_multitexcoord0;
static GLint unif_color;
static GLint unif_palette;
static GLint unif_texture;

struct demo4_state {
	unsigned tick;
	GLuint program;
	GLuint buf;
	GLuint palette_tex;
	GLuint palette_buf[256];
	GLubyte *screen_buf;
	GLuint screen_tex;
	GLsizei screen_width;
	GLsizei screen_height;
} state;

/**********************************************************************/

static int
compile_shader(GLuint *program_out, const GLchar *vert_source, const GLchar *frag_source)
{
	initgl_gl_check();

	*program_out = 0; /* initialize assuming failure */

	// vertex shader
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vert_source, 0);
	glCompileShader(vshader);

	initgl_gl_check();

	// fragment shader
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &frag_source, 0);
	glCompileShader(fshader);

	GLuint shader_parts[] = { vshader, fshader };

	initgl_gl_check();

	unsigned i;
	for (i = 0; i < sizeof(shader_parts) / sizeof(*shader_parts); i++) {
		GLint compile_status;
		glGetShaderiv(shader_parts[i], GL_COMPILE_STATUS, &compile_status);
		if (compile_status != GL_TRUE) {
			GLint info_len = 0;
			glGetShaderiv(shader_parts[i], GL_INFO_LOG_LENGTH, &info_len);
			if (info_len > 255) {
				info_len = 255;
			}
			char info[256];
			glGetShaderInfoLog(shader_parts[i], info_len, NULL, info);
			log_error("GL shader #%u compile failure:%s", i, info);
			return ERR;
		}
	}

	initgl_gl_check();

	GLuint program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	GLint link_status;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		GLint info_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 255) {
				info_len = 255;
		}
		char info[256];
		glGetProgramInfoLog(program, info_len, NULL, info);
		log_error("GL program link failure:%s", info);
		return ERR;
	}

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	initgl_gl_check();

	*program_out = program;

	return OK;
}

static int
load_attributes(GLuint program)
{
	attr_vertex = glGetAttribLocation(program, "vertex");
	attr_multitexcoord0 = glGetAttribLocation(program, "AttrMultiTexCoord0");
	unif_color = glGetUniformLocation(program, "color");
	unif_palette = glGetUniformLocation(program, "palette");
	unif_texture = glGetUniformLocation(program, "texture");

	if (attr_vertex < 0) {
		log_error("no vertex attribute");
		return ERR;
	}

	if (unif_color < 0) {
		log_error("WARN:no color uniform");
	}

	if (unif_palette < 0) {
		log_error("WARN:no palette uniform");
	}

	if (unif_texture < 0) {
		log_error("WARN:no screen texture uniform");
	}

	return OK;
}

static int
load_buffer(GLuint *buf_out, GLuint *screen_tex_out, int screen_width, int screen_height, GLuint *pal_tex_out)
{
	if (!buf_out) {
		errno = EINVAL;
		return ERR;
	}

	/*** Vertex Buffer ***/

	static const GLfloat vertex_data[] = {
		-1.0, -1.0, 1.0, 1.0,
		1.0, -1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0, 1.0
	};

	GLuint buf = 0;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(attr_vertex, 4, GL_FLOAT, 0, 16, 0);
	glEnableVertexAttribArray(attr_vertex);
	*buf_out = buf;

	initgl_gl_check();

	/*** Textures ***/

	// Screen texture
	GLuint screen_tex = 0;
	glGenTextures(1, &screen_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, screen_width, screen_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
	*screen_tex_out = screen_tex;

	// Palette texture
	/* R/G/B */
	GLuint palette_tex = 0;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &palette_tex);
	glBindTexture(GL_TEXTURE_2D, palette_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#ifndef USE_GLES2 // GLES 3+ / OpenGL 3+
	// TODO: untested code path!
	const GLint lsbred_no_alpha_swizzle[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ONE };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, lsb_no_alpha_swizzle);
#endif
	*pal_tex_out = palette_tex;

	return OK;
}

/* initialize 256 color palette */
static void
palette_init(GLuint palette[])
{
	unsigned i;
	unsigned char r, g, b;
	GLuint c;

	/* traditional TTL RGB : colors 0~15 */
	i = 0;
	palette[i++] = 0x000000; // 0 - black
	palette[i++] = 0x800000; // 1 - red
	palette[i++] = 0x008000; // 2 - green
	palette[i++] = 0x808000; // 3 - brown
	palette[i++] = 0x000080; // 4 - blue
	palette[i++] = 0x800080; // 5 - magenta
	palette[i++] = 0x008080; // 6 - cyan
	palette[i++] = 0xc0c0c0; // 7 - white
	/* high-intensity (bright mode) */
	palette[i++] = 0x808080; // 8 - dark grey
	palette[i++] = 0xff0000; // 9 - bright red
	palette[i++] = 0x00ff00; // 10 - bright green
	palette[i++] = 0xffff00; // 11 - yellow
	palette[i++] = 0x0000ff; // 12 - bright blue
	palette[i++] = 0xff00ff; // 13 - bright magenta
	palette[i++] = 0x00ffff; // 14 - bright cyan
	palette[i++] = 0xffffff; // 15 - bright white

	/* RGB 6x6x6 color cube : colors 16~231 */
	for (; i < 232; i++) {
		unsigned n = i - 16;

		b = n % 6;
		g = n / 6 % 6;
		r = n / 36 % 6;

		c = r * 255 / 5;
		c <<= 8;
		c |= g * 255 / 5;
		c <<= 8;
		c |= b * 255 / 5;

		palette[i] = c;
	}

	/* grey scale : colors 232~255 */
	for (; i < 256; i++) {
		unsigned n = i - 232;
		/* 08, 12, 1c, 26, ... d0, da, e4, ee */
		c = 8 + n * 230 / 23;
		palette[i] = c | (c << 8) | (c << 16);
	}
}

static int
screen_update(unsigned left, unsigned top, unsigned right, unsigned bottom)
{
	GLsizei width = right - left + 1;
	GLsizei height = bottom - top + 1;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.screen_tex);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifdef USE_GLES2 // GLES 2
	glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, state.screen_buf);
#else // OpenGL 3+
	// TODO: untested code path!
	const GLint one_byte_swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, one_byte_swizzle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, width, height, GL_RED, GL_UNSIGNED_BYTE, state.screen_buf);
#endif
	return OK;
}

static int
screen_init(int screen_width, int screen_height)
{
	state.screen_width = screen_width;
	state.screen_height = screen_height;
	state.screen_buf = malloc(screen_width * screen_height);
	if (!state.screen_buf) {
		return ERR;
	}

	memset(state.screen_buf, 0, screen_width * screen_height);

	return OK;
}

/* load shader, initialize GL state */
static int
my_gl_init(void)
{
	/* vertex shader source */
	const GLchar vert_source[] = {
		"#version 100 // GLESv1\n"
		"precision mediump float;\n"
		"attribute vec4 vertex;\n"
		// "attribute vec4 AttrMultiTexCoord0;\n"
		"varying vec2 VaryingTexCoord0;\n"
		"void main(void) {\n"
		// "  VaryingTexCoord0 = AttrMultiTexCoord0.xy;\n"
		"  VaryingTexCoord0 = vertex.xy * 0.5 + 0.5;\n"
		"  gl_Position = vertex * 0.5;\n"
		"}\n"
	};
	/* fragment shader source */
	const GLchar frag_source[] = {
		"#version 100 // GLESv1\n"
		"precision mediump float;\n"
		"uniform sampler2D texture;\n"
		"uniform sampler2D palette;\n"
		"varying vec2 VaryingTexCoord0;\n"
		"void main(void) {\n"
		"  gl_FragColor = texture2D(palette, vec2(texture2D(texture, VaryingTexCoord0).r, 0));\n"
		// "  gl_FragColor = texture2D(palette, VaryingTexCoord0);\n"
		"}\n"
	};

	if (compile_shader(&state.program, vert_source, frag_source) != OK) {
		return ERR;
	}
	assert(state.program != 0);

	initgl_gl_check();

	load_attributes(state.program);

	initgl_gl_check();

	glUseProgram(state.program);

	initgl_gl_check();

	screen_init(320, 240);

	/* attr_vertex, etc. must be set before calling this */
	load_buffer(&state.buf, &state.screen_tex, state.screen_width,
		    state.screen_height, &state.palette_tex);

	// TODO: place this in load_palette()
	palette_init(state.palette_buf);
	glBindTexture(GL_TEXTURE_2D, state.palette_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, state.palette_buf);

	screen_update(0, 0, state.screen_width - 1, state.screen_height -1);

	initgl_gl_check();

	return OK;
}

static void
my_gl_done(void)
{
	if (state.program) {
		glUseProgram(0);
		glDeleteProgram(state.program);
		state.program = 0;
	}

	if (state.buf) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &state.buf);
		state.buf = 0;
	}
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
screen_paint(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, state.buf);
	glUseProgram(state.program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.screen_tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, state.palette_tex);

	if (unif_color >= 0) {
		glUniform4f(unif_color, 0.5, 0.5, 0.8, 1.0);
	}
	if (unif_palette >= 0) {
		glUniform1i(unif_palette, 1); // palette_tex is GL_TEXTURE1
	}
	if (unif_texture >= 0) {
		glUniform1i(unif_texture, 0); // screen_tex is GL_TEXTURE0
	}

	initgl_gl_check();

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	/* clean up state */

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	initgl_gl_check();
}

static void
paint(void)
{
	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	screen_paint();

	glFlush();

	initgl_gl_check();
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

static void
animate(void)
{
	unsigned tick = state.tick++;

	GLsizei x, y;
	unsigned row_bytes = state.screen_width;
	GLubyte *screen_buf = state.screen_buf;

	if (!screen_buf) { return; }

	for (y = 0; y < state.screen_height; y++) {
		GLubyte *row = state.screen_buf + y * row_bytes;
		for (x = 0; x < state.screen_width; x++) {
			row[x] = ((x + (y ^ 12) + tick) % 16) + 232;
		}
	}

	screen_update(0, 0, state.screen_width - 1, state.screen_height -1);
}

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

	if (my_gl_init() != OK) {
		return 1;
	}

	while (!terminate_flag) {
		process_events();
		animate();
		paint_all();
	}

	my_gl_done();
	display_done();

	return 0;
}