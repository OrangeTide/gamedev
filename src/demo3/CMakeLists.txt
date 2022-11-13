cmake_minimum_required( VERSION 3.22.1 )
project( demo3 )

set( demo3_sources
	demo3.c
	input.c
	graphics.c
	gui.c
	screen.c
	timer.c
	)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
	#######################
	#### Android Build ####
	#######################

	add_library( demo3 SHARED ${demo3_sources} )

	target_compile_options( demo3
		PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11 -Wall -Werror>
		)

	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	target_link_options( demo3
		PRIVATE "-u ANativeActivity_onCreate"
		)

	# add lib dependencies
	target_link_libraries( demo3
		PRIVATE sokol
		PRIVATE cimgui
		PRIVATE android
		PRIVATE EGL
		PRIVATE GLESv3
		PRIVATE OpenSLES
		PRIVATE log
		)

	# the library name must match the one used in android/app/src/main/AndroidManifest.xml
	set_target_properties(demo3 PROPERTIES OUTPUT_NAME "native-activity")
else()
	#######################################
	#### Desktop Build (Linux/Windows) ####
	#######################################

	add_executable(demo3 ${demo3_sources})

	target_compile_options( demo3
		PRIVATE -Wall -W -O2
		)
endif()

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
