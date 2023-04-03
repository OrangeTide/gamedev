cmake_minimum_required( VERSION 3.22.1 )
project( hellodungeon )

set( hellodungeon_sources
	hellodungeon.c
	input.c
	graphics.c
	gui.c
	timer.c
	)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
	#######################
	#### Android Build ####
	#######################

	add_library( hellodungeon SHARED ${hellodungeon_sources} )

	target_compile_options( hellodungeon
		PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11 -Wall -Werror>
		)

	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	target_link_options( hellodungeon
		PRIVATE "-u ANativeActivity_onCreate"
		)

	# add lib dependencies
	target_link_libraries( hellodungeon
		PRIVATE sokol
		PRIVATE cimgui
		PRIVATE android
		PRIVATE EGL
		PRIVATE GLESv3
		PRIVATE OpenSLES
		PRIVATE log
		)

	# TODO: the library name must match the one used in android/app/src/main/AndroidManifest.xml
else()
	#######################################
	#### Desktop Build (Linux/Windows) ####
	#######################################

	add_executable(hellodungeon ${hellodungeon_sources})

	target_compile_options( hellodungeon
		PRIVATE -Wall -W -O2
		)
endif()

target_compile_definitions( hellodungeon
	PRIVATE NTEST
	PRIVATE NDEBUG
	)

target_link_libraries( hellodungeon
	PRIVATE sokol
	PRIVATE cimgui
	)

target_include_directories( hellodungeon
	PUBLIC .
	)
