add_executable( demo1 demo1.c )

target_compile_options( demo1
	PRIVATE -Wall -W -O2
	PUBLIC -flto -g
	)

target_compile_definitions( demo1
	PRIVATE NTEST
	PRIVATE NDEBUG
	)

target_link_libraries( demo1
        PRIVATE sokol
	)

target_include_directories( demo1
	PUBLIC .
	)
