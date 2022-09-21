#!/bin/bash
set -xe

# Sokol (master)
curl -O https://raw.githubusercontent.com/floooh/sokol/master/sokol_app.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/sokol_audio.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/sokol_glue.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/sokol_gfx.h \
	-O https://raw.githubusercontent.com/floooh/sokol/master/util/sokol_imgui.h

# Apply patches
rm sokol_gamepad.h
patch -p1 < 0001-gamepad-API.patch
patch -p1 < 0002-sokol_app-changes-for-android-support.patch
patch -p1 < 0003-fix-initializer-in-gamepad-API.patch
patch -p1 < 0004-load-XInput-library-in-gamepad-API.patch
