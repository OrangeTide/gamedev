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

	# build native_app_glue as a static lib
	set( ${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}" )
	add_library( native_app_glue STATIC
		${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
		)

	# now build app's shared lib
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror" )

	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	set( CMAKE_SHARED_LINKER_FLAGS
		"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
		)

	add_library( demo3 SHARED ${demo3_sources} )

	target_include_directories( demo3 PRIVATE
		${ANDROID_NDK}/sources/android/native_app_glue )

	# add lib dependencies
	target_link_libraries( demo3
		PRIVATE sokol
		PRIVATE cimgui
		PRIVATE android
		#NOT NEEDED:	PRIVATE native_app_glue
		PRIVATE EGL
		PRIVATE GLESv3
		PRIVATE OpenSLES
		PRIVATE log
		)

	# the app glue and manifests assume the name is libnative-activity.so
	set_target_properties(demo3 PROPERTIES OUTPUT_NAME "native-activity")
else()
	#######################################
	#### Desktop Build (Linux/Windows) ####
	#######################################

	add_executable(demo3 ${demo3_sources})

	target_compile_options( demo3
		PRIVATE -Wall -W -O2
		PUBLIC -flto -g
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
