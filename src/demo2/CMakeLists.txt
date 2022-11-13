add_executable( demo2 demo2.c )

target_compile_options( demo2
	PRIVATE -Wall -W -O2
	)

target_compile_definitions( demo2
	PRIVATE NTEST
	PRIVATE NDEBUG
	)

target_link_libraries( demo2
        PRIVATE sokol
	)

target_include_directories( demo2
	PUBLIC .
	)
