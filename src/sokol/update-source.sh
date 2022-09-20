#!/bin/bash
set -xe

# Sokol (master)
curl -O https://raw.githubusercontent.com/floooh/sokol/master/sokol_app.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/sokol_audio.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/sokol_glue.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/sokol_gfx.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/util/sokol_imgui.h
