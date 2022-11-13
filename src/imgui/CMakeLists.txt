cmake_minimum_required( VERSION 3.22.1 )
project( imgui-static )

add_library( imgui STATIC
	imgui/imgui.cpp
	imgui/imgui.h
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_internal.h
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
	)
target_include_directories( imgui INTERFACE . )

add_library( cimgui STATIC
	cimgui/cimgui.cpp
	cimgui/cimgui.h
	)
target_include_directories( cimgui INTERFACE . )
target_link_libraries( cimgui PUBLIC imgui )
