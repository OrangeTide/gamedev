add_executable( demo1 demo1.c )

target_compile_options( demo1
	PRIVATE -Wall -W -O2
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

if (EMSCRIPTEN)
add_custom_command(
	TARGET demo1 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/demo1.html
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/demo1.html
	)
endif()
