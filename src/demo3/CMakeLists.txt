add_executable( demo3
	demo3.c
	input.c
	graphics.c
	gui.c
	screen.c
	timer.c
	)

target_compile_options( demo3
	PRIVATE -Wall -W -O2
	PUBLIC -flto -g
	)

target_compile_definitions( demo3
	PRIVATE NTEST
	PRIVATE NDEBUG
	)

target_link_libraries( demo3
	PRIVATE sokol
	PRIVATE cimgui
	)

target_include_directories( demo3
	PUBLIC .
	)
