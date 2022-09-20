add_library(sokol STATIC
	sokol.c
	sokol_gfx.h
	sokol_app.h
	sokol_audio.h
	sokol_glue.h
	)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    # compile sokol.c as Objective-C
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal"
        "-framework OpenGL"
        "-framework AudioToolbox")
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL asound dl m)
    target_link_libraries(sokol PUBLIC Threads::Threads)
endif()
target_link_libraries(sokol PRIVATE cimgui)
target_include_directories(sokol INTERFACE .)
