#!/bin/bash
set -xe

IMGUI_VERSION=1.87

# ImGUI
mkdir -p imgui ; cd imgui
curl -O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui.cpp \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui.h \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui_demo.cpp \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui_draw.cpp \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui_internal.h \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui_tables.cpp \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imgui_widgets.cpp \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imstb_rectpack.h \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imstb_textedit.h \
	-O https://raw.githubusercontent.com/ocornut/imgui/v${IMGUI_VERSION}/imstb_truetype.h

touch imconfig.h
cd ..

# cimgui
mkdir -p cimgui ; cd cimgui
curl -O https://raw.githubusercontent.com/cimgui/cimgui/${IMGUI_VERSION}/cimgui.cpp \
	-O https://raw.githubusercontent.com/cimgui/cimgui/${IMGUI_VERSION}/cimgui.h
cd ..
