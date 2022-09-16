#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_audio.h"
#include "sokol_glue.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "triangle-sapp.glsl.h"

/***********************************************************************
 * Prototypes
 ***********************************************************************/
static void game_tick(void);

static void gfx_init(void);
static void gfx_done(void);
static void gfx_draw(void);

/***********************************************************************
 * State
 ***********************************************************************/

#define TRANSLATE_KEYS_MAX 512			// maximum number of key codes, see enum sapp_keycode
#define INPUT_ACTION_MAX 16			// number of input actions, see enum input_action
#define TICK_TOLERANCE_NS 1000000		// per-frame tolerance in nanoseconds
#define TICK_DURATION_NS (100000000 / 60)	// resolution of tick-based events (virtual frame time)
#define TIMER_DISABLED 0xfffffffful		// magic tick value for a disabled timer

enum input_action {
	INPUT_ACTION_NONE,
	INPUT_ACTION_UP,
	INPUT_ACTION_DOWN,
	INPUT_ACTION_LEFT,
	INPUT_ACTION_RIGHT,
};

enum gamestate {
	GAMESTATE_PLAYING,
};

struct state {
	enum gamestate gamestate;
	struct input_state {
		/** globally enables/disables action input */
		bool enabled;
		/** indicates new keys since last read */
		bool fresh_keys;
		/** true if an action is active (key is held down) */
		bool action_down[INPUT_ACTION_MAX];
		/** Translates SAPP_KEYCODE_x to one of the action_down[] states */
		unsigned char translate[TRANSLATE_KEYS_MAX];
	} input;

	struct timing {
		unsigned tick;
		int accumulator;
	} timing;

	struct game {
		unsigned timer_started;
	} game;

	struct gfx {
		sg_pass_action pass_action;
		sg_pipeline pip;
		sg_bindings bind;
	} gfx;

};

static struct state state = {
	.gamestate = GAMESTATE_PLAYING,
	.input = {
		.enabled = true,
		.translate = {
			[SAPP_KEYCODE_UP] = INPUT_ACTION_UP,
			[SAPP_KEYCODE_W] = INPUT_ACTION_UP,
			[SAPP_KEYCODE_DOWN] = INPUT_ACTION_DOWN,
			[SAPP_KEYCODE_S] = INPUT_ACTION_DOWN,
			[SAPP_KEYCODE_LEFT] = INPUT_ACTION_LEFT,
			[SAPP_KEYCODE_A] = INPUT_ACTION_LEFT,
			[SAPP_KEYCODE_RIGHT] = INPUT_ACTION_RIGHT,
			[SAPP_KEYCODE_D] = INPUT_ACTION_RIGHT,
		},
	},
	.gfx.pass_action = {
		.colors[0] = {
			.action = SG_ACTION_CLEAR,
			.value = { 0.0f, 0.0f, 0.0f, 1.0f }
		}
	},
};

static void
timer_stop(unsigned *t)
{
	*t = TIMER_DISABLED;
}

static void
timer_start(unsigned *t)
{
	*t = state.timing.tick;
}

static bool
timer_now(unsigned t)
{
	return state.timing.tick == t;
}

/***********************************************************************
 * Base
 ***********************************************************************/

static void
init(void)
{
	gfx_init();
}

static void
done(void)
{
	gfx_done();
}

static void
frame_update(void)
{
	unsigned frame_time_ns = (unsigned)(sapp_frame_duration() * 1000000000.0);

	if (frame_time_ns > 33333333) {
		frame_time_ns = 33333333;
	}

	state.timing.accumulator += frame_time_ns;
	while(state.timing.accumulator > -TICK_TOLERANCE_NS) {
		if (timer_now(state.game.timer_started)) {
		    state.gamestate = GAMESTATE_PLAYING;
		}

		switch (state.gamestate) {
		case GAMESTATE_PLAYING:
			game_tick();
		}

		state.timing.tick++;
		state.timing.accumulator -= TICK_DURATION_NS;
	}

	gfx_draw();
}

static void
input(const sapp_event *ev)
{
	if (!state.input.enabled)
		return;
	switch (ev->type) {
	case SAPP_EVENTTYPE_KEY_DOWN:
	case SAPP_EVENTTYPE_KEY_UP:
		if (ev->key_code > SAPP_KEYCODE_INVALID
		    && ev->key_code < TRANSLATE_KEYS_MAX) {
			state.input.action_down[state.input.translate[ev->key_code]]
				= ev->type == SAPP_EVENTTYPE_KEY_DOWN;
			state.input.fresh_keys = true;
		}
		break;
	default:
		// ignored
	}
}

sapp_desc
sokol_main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	sapp_desc desc = {
		.window_title = "demo1",
		.init_cb = init,
		.cleanup_cb = done,
		.frame_cb = frame_update,
		.event_cb = input,
		.width = 640,
		.height = 480,
	};

	return desc;
}

/***********************************************************************
 * Game
 ***********************************************************************/

static void
game_tick(void)
{
}

/***********************************************************************
 * Graphics
 ***********************************************************************/

static void
gfx_init(void)
{
	const sg_desc desc = {
		/*
		.buffer_pool_size = 2,
		.image_pool_size = 3,
		.shader_pool_size = 2,
		.pipeline_pool_size = 2,
		.pass_pool_size = 1,
		*/
		.context = sapp_sgcontext()
	};
	sg_setup(&desc);

	float vertices[] = {
		// positions            // colors
		 0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
	};

	sg_buffer_desc vbuffer = {
		.data = SG_RANGE(vertices),
		.type = SG_BUFFERTYPE_VERTEXBUFFER,
		.usage = SG_USAGE_IMMUTABLE,
		.label = "triangle-vertices",
	};
	state.gfx.bind.vertex_buffers[0] = sg_make_buffer(&vbuffer);

	sg_shader shd = sg_make_shader(demo1_triangle_shader_desc(sg_query_backend()));

	sg_pipeline_desc pipeline_desc = {
		.shader = shd,
		.layout = {
			.attrs = {
				[ATTR_demo1_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
				[ATTR_demo1_vs_color0].format = SG_VERTEXFORMAT_FLOAT4
			}
		},
		.label = "triangle-pipeline"
	};

	state.gfx.pip = sg_make_pipeline(&pipeline_desc);
}

static void
gfx_done(void)
{
	sg_shutdown();
}

static void
gfx_draw(void)
{
	const int canvas_width = sapp_width();
	const int canvas_height = sapp_height();

	sg_begin_default_pass(&state.gfx.pass_action, canvas_width, canvas_height);
	sg_apply_pipeline(state.gfx.pip);
	sg_apply_bindings(&state.gfx.bind);
	sg_draw(0, 3, 1);
	sg_end_pass();
	sg_commit();
}
