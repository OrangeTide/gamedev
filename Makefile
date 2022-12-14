# -----------------------------------------------------------------------------
# CMake project wrapper Makefile ----------------------------------------------
# -----------------------------------------------------------------------------

SHELL := /bin/bash
RM    := rm -f
RMF   := rm -rf
MKDIR := mkdir -p
RMDIR := rmdir
CMAKE := cmake

# detect host build system
ifneq ($(MINGW_CHOST),)
CMAKE_OPTS := -G "MinGW Makefiles"
else
CMAKE_OPTS := -G "Unix Makefiles"
endif

# Emscripten - EXPERIMENTAL
ifneq ($(EMSCRIPTEN),)
CMAKE := emcmake cmake
endif

.PHONY: all clean distclean

all: ./build/Makefile
	@ $(MAKE) -C build

./build/Makefile:
	@  ($(MKDIR) build > /dev/null)
	@  (cd build > /dev/null 2>&1 && $(CMAKE) $(CMAKE_OPTS) -DCMAKE_VERBOSE_MAKEFILE=TRUE .. )

clean: ./build/Makefile
	@- $(MAKE) -C build clean || true

distclean:
	@  echo Removing build/
	@  ($(MKDIR) build > /dev/null)
	@  (cd build > /dev/null 2>&1 && $(CMAKE) $(CMAKE_OPTS) .. > /dev/null 2>&1)
	@- $(MAKE) --silent -C build clean || true
	@- $(RM) ./build/Makefile
	@- $(RMF) ./build/CMake*
	@- $(RM) ./build/cmake.*
	@- $(RM) ./build/*.cmake
	@- $(RM) ./build/*.txt
	@- $(RMF) ./build/test
	@- $(RMF) ./build/src
	@  $(RMDIR) ./build

ifeq ($(findstring distclean,$(MAKECMDGOALS)),)
$(MAKECMDGOALS): ./build/Makefile
	@ $(MAKE) -C build $(MAKECMDGOALS)
endif

server: all
	@ echo Connect to http://localhost:8080/demo1.html
	@ cd bin ; python3 -m http.server 8080 --bind 127.0.0.1
