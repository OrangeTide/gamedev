/* input.c */
#include "input.h"
#include "state.h"
#include "gui.h"
#include <sokol_app.h>

void
input(const sapp_event *ev)
{
	if (gui_event(ev)) {
		return;
	}
	if (!state.input.enabled)
		return;
	switch (ev->type) {
	case SAPP_EVENTTYPE_KEY_DOWN:
		if (ev->key_code == SAPP_KEYCODE_ESCAPE) {
			sapp_request_quit();
			break;
		}
		// fallthru
	case SAPP_EVENTTYPE_KEY_UP:
		if (ev->key_code > SAPP_KEYCODE_INVALID
		    && ev->key_code < TRANSLATE_KEYS_MAX) {
			state.input.action_down[state.input.translate[ev->key_code]]
				= ev->type == SAPP_EVENTTYPE_KEY_DOWN;
			state.input.fresh_keys = true;
		}
		break;
	default:
		; // ignored
	}
}
