# Jon's Modular Makefile
# You are free to modify, rename, steal, or redistribute this file.
# Version: December 2023
############################################################################
# Usage:
# 1. Modify PROJECT_DIRS variable in this file to indicate source directories.
# 2. Create a .prj file in each source directory describing your project(s).
# 3. Run `make` (must be GNU make)
#    a. optionally specify a config: `make CONFIG=configs/yourconfig.mk`
# 4. Look in bin/ or build/ for your executables and libraries
# 5. implement your tests in tests/test-somename.sh.
#    a. run tests with `make test` or `make test-somename`
############################################################################
# TODO: support building shared libraries (TYPE=dll)
#
############################################################################
TOP := $(dir $(lastword $(call fixpath,${MAKEFILE_LIST})))
PROJECT_DIRS := src
# alternative for small projects : PROJECT_DIRS := .

# Save default compilers, assume these are for the Host's toolchain.
HOSTCC := $(CC)
HOSTCXX := $(CXX)
CC = ${TOOLCHAIN_PREFIX}gcc
CXX = ${TOOLCHAIN_PREFIX}g++
LD = ${TOOLCHAIN_PREFIX}ld
AS = ${TOOLCHAIN_PREFIX}as
AR = ${TOOLCHAIN_PREFIX}ar
OBJDUMP = ${TOOLCHAIN_PREFIX}objdump
OBJCOPY = ${TOOLCHAIN_PREFIX}objcopy

ifeq ($(CONFIG),)
-include config.mk
else
include $(CONFIG)
endif

## Host OS and Architecture detection

ifeq ($(OS),)
OS := $(shell uname -s)
endif

ifeq ($(ARCH),)
ifeq ($(OS),Windows_NT)
# TODO: detect architecture on Windows
ARCH := x86_64
else
ARCH := $(shell uname -m)
endif
endif

ifeq ($(OS),Windows_NT)
TOOLCHAIN_OS=win32
else ifeq ($(OS),Linux)
TOOLCHAIN_OS=linux
else ifeq ($(OS),Darwin)
# TODO: detect legacy. TOOLCHAIN_OS=osx
TOOLCHAIN_OS=osx_arm64
else
$(error Unsupported operating system $(OS))
endif

ifeq ($(findstring sh,$(SHELL)),sh)
# detect a unix style shell
MKDIR := mkdir -p
RM = rm -f
CP := cp -f
fixpath = $1
else
# Requires "setlocal enableextensions"
MKDIR := mkdir
RM := del /Q
CP := copy
fixpath = $(subst /,\,$1)
endif
RMDIR := rmdir

rmdir = $(if $1,$(RMDIR) $1)

# platform config
TARGET_OS ?= $(OS)
TARGET_ARCH ?= $(ARCH)

ifeq ($(TARGET_OS),Windows_NT)
EXTENSION.exe = .exe
EXTENSION.elf = .elf
EXTENSION.dll = .dll
EXTENSION.lib = .a
EXTENSION.bin = .bin
else
EXTENSION.exe =
EXTENSION.elf = .elf
EXTENSION.dll = .so
EXTENSION.lib = .a
EXTENSION.bin = .bin
endif

EXTENSION.header = TBD

# enable dependency generation for gcc
CCOPTS.gcc = -MMD

# TODO: detect compiler
CCOPTS = $(CCOPTS.gcc)

## Core Makefile

EXTENSIONS := c cpp S

.SUFFIXES:
.SUFFIXES: $(addprefix .,$(EXTENSIONS))

BUILDDIR := ${TOP}build/$(TARGET_OS)-$(TARGET_ARCH)/
TARGETDIR.exe := ${TOP}bin/$(TARGET_OS)-$(TARGET_ARCH)/
TARGETDIR.elf := ${TOP}bin/$(TARGET_OS)-$(TARGET_ARCH)/
TARGETDIR.dll := ${TOP}bin/$(TARGET_OS)-$(TARGET_ARCH)/
TARGETDIR.bin := ${TOP}bin/$(TARGET_OS)-$(TARGET_ARCH)/
TARGETDIR.lib := ${BUILDDIR}lib/

all ::
clean ::
clean-bins :: ; $(RM) $(ALL_DEPS)
clean-all :: clean clean-bins ; $(clean.cmd)
clean-project ::
.PHONY : all clean clean-all clean-bins distclean test
distclean : clean-all
	-rmdir $(BUILDDIR)
	-rmdir $(dir $(BUILDDIR))
	-rmdir $(TARGETDIR.exe)
	-rmdir $(dir $(TARGETDIR.exe))
default : all
test ::
test-% : ; tests/test-$*.sh ${_EXEC.$*}

config.mk :
	@echo "Select a configuration from configs/ and copy into config.mk"
	@ls -1 configs/*.mk
	@false

# build actions
link.exe = $(if ${CXX_MODE},$(CXX),$(CC)) -o $@ $(LDFLAGS) $^ $(LDLIBS)
link.elf = $(if ${CXX_MODE},$(CXX),$(CC)) -o $@ $(LDFLAGS) $(if $(LINKERFILE),-T $(LINKERFILE)) $^ $(LDLIBS)
# TODO: support building Windows dll
link.dll = $(if ${CXX_MODE},$(CXX),$(CC)) -o $@ -shared -Wl,-soname=$@.0 $(LDFLAGS) $^ $(LDLIBS)
link.bin = $(if ${CXX_MODE},$(CXX),$(CC)) -o $@ $(LDFLAGS) $^ $(LDLIBS)
link.lib = $(if $^,$(AR) $(ARFLAGS) $@ $^,$(error No files for archive))
compile.c = $(CC) -c -o $@ $(CCOPTS) $(CPPFLAGS) $(CFLAGS) $<
compile.cpp = $(CXX) -c -o $@ $(CCOPTS) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $<
compile.S = $(CC) -c -o $@ $(CPPFLAGS) $(ASFLAGS) $<
copy = $(CP) $< $@

## macros
# reverse a list:
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))
# recursive wildcard
rwildcard = $(foreach d,$(wildcard $(addsuffix /*,$(1))),$(call rwildcard,$(d),$(2))$(filter $(subst *,%,$(2)),$(d)))
# explode a path into a series of subdirectories
subby = $(sort $(filter-out .,$(if $(1),$(call subby,$(filter-out $(1),$(patsubst %/,%,$(dir $(1)))))) $(patsubst %/,%,$(1))))

to-ext = $(foreach X,${EXTENSIONS},$(patsubst %.$X,%.$1,$(filter %.$X,$2)))
set-sources = $(foreach X,${EXTENSIONS},$(eval _SRCS_$X.$1 := $(filter %.$X,$2)))

undefine-many = $(foreach V,$1,$(eval undefine $V))
undefine-many-os = $(call undefine-many,$1 $(addsuffix .${OS},$1) $(addsuffix .${OS}.${ARCH},$1) $(addsuffix .${ARCH},$1))

# clear project config variables
define clear-vars
$(call undefine-many,NAME EXEC TYPE)
$(call undefine-many-os,SRCS OBJS CFLAGS CXXFLAGS CPPFLAGS LDFLAGS LDLIBS ASFLAGS USES EXTRA INCLUDEDIR LIBDIR LINKERFILE COPYFILES)
endef

define begin-project
$(eval CURRENT_PROJECT_DIR := $(dir $(lastword ${MAKEFILE_LIST})))
HERE := ${CURRENT_PROJECT_DIR}
$(info Project dir: $(CURRENT_PROJECT_DIR))
endef

TARGET_LIST :=

# store NAME, TYPE, SRCS, OBJS, etc to project database
define store-project
$(eval $(if ${NAME},,$(error NAME is not defined in $(lastword ${MAKEFILE_LIST}))))
$(eval $(if ${TYPE},,$(error TYPE is not defined in $(lastword ${MAKEFILE_LIST}) for ${NAME})))
_BASEDIR.${NAME} := ${CURRENT_PROJECT_DIR}
$(eval _TYPE.${NAME} := $${TYPE})
ifeq ($(_TYPE.${NAME}),header)
_EXEC.${NAME} :=
else
TARGET_LIST := ${TARGET_LIST} ${NAME}
_EXEC_BASE.${NAME} := $(if ${EXEC},${EXEC},${NAME})
$(eval _EXEC.${NAME} := ${TARGETDIR.${_TYPE.$(NAME)}}${if ${EXEC},${EXEC},$(NAME)}${EXTENSION.${_TYPE.$(NAME)}})
endif
$(eval _CFLAGS.${NAME} := ${CFLAGS} ${CFLAGS.$(TARGET_OS)} ${CFLAGS.$(TARGET_ARCH)} ${CFLAGS.$(TARGET_OS).$(TARGET_ARCH)})
$(eval _CXXFLAGS.${NAME} := ${CXXFLAGS} ${CXXFLAGS.$(TARGET_OS)} ${CXXFLAGS.$(TARGET_ARCH)} ${CXXFLAGS.$(TARGET_OS).$(TARGET_ARCH)})
$(eval _CPPFLAGS.${NAME} := \
	${CPPFLAGS} ${CPPFLAGS.$(TARGET_OS)} ${CPPFLAGS.$(TARGET_ARCH)} ${CPPFLAGS.$(TARGET_OS).$(TARGET_ARCH)} \
	$(addprefix -I${CURRENT_PROJECT_DIR}, \
	${INCLUDEDIR} ${INCLUDEDIR.$(TARGET_OS)} ${INCLUDEDIR.$(TARGET_ARCH)} ${INCLUDEDIR.$(TARGET_OS).$(TARGET_ARCH)}) \
	-I${CURRENT_PROJECT_DIR})
$(eval _LDFLAGS.${NAME} := ${LDFLAGS} ${LDFLAGS.$(TARGET_OS)} ${LDFLAGS.$(TARGET_ARCH)} ${LDFLAGS.$(TARGET_OS).$(TARGET_ARCH)})
$(eval _LDLIBS.${NAME} := ${LDLIBS} ${LDLIBS.$(TARGET_OS)} ${LDLIBS.$(TARGET_ARCH)} ${LDLIBS.$(TARGET_OS).$(TARGET_ARCH)})
_ASFLAGS.${NAME} := ${ASFLAGS} ${ASFLAGS.$(TARGET_OS)} ${ASFLAGS.$(TARGET_ARCH)} ${ASFLAGS.$(TARGET_OS).$(TARGET_ARCH)}
_LINKERFILE.${NAME} := ${LINKERFILE} ${LINKERFILE.$(TARGET_OS)} ${LINKERFILE.$(TARGET_ARCH)} ${LINKERFILE.$(TARGET_OS).$(TARGET_ARCH)}
_OBJS.${NAME} := ${OBJS} ${OBJS.$(TARGET_OS)} ${OBJS.$(TARGET_ARCH)} ${OBJS.$(TARGET_OS).$(TARGET_ARCH)}
_USES.${NAME} := ${USES} ${USES.$(TARGET_OS)} ${USES.$(TARGET_ARCH)} ${USES.$(TARGET_OS).$(TARGET_ARCH)}
$(eval _COPYFILES.${NAME} := \
	$(if ${COPYFILES}${COPYFILES.$(TARGET_OS)}${COPYFILES.$(TARGET_ARCH)}${COPYFILES.$(TARGET_OS).$(TARGET_ARCH)}, \
	$(addprefix ${CURRENT_PROJECT_DIR}, \
	${COPYFILES} ${COPYFILES.$(TARGET_OS)} ${COPYFILES.$(TARGET_ARCH)} ${COPYFILES.$(TARGET_OS).$(TARGET_ARCH)}), \
	))
ifeq ($${TYPE},lib)
$(eval _PROVIDES_LDLIBS.${NAME} = ${_LDLIBS.$(NAME)} \
	$$(foreach u,$${_USES.$(NAME)},$${_EXEC.$$u} $${_LDLIBS.$$u}))
_PROVIDES_LDFLAGS.${NAME} := ${_LDFLAGS.$(NAME)} \
	$(foreach u,${_USES.$(NAME)},$${_PROVIDES_LDFLAGS.$u}) \
	$(if ${LIBDIR}${LIBDIR.$(TARGET_OS)}${LIBDIR.$(TARGET_ARCH)}${LIBDIR.$(TARGET_OS).$(TARGET_ARCH)}, \
	$(addprefix -L${CURRENT_PROJECT_DIR}, \
	${LIBDIR} ${LIBDIR.$(TARGET_OS)} ${LIBDIR.$(TARGET_ARCH)} ${LIBDIR.$(TARGET_OS).$(TARGET_ARCH)}), \
	)
_PROVIDES_CFLAGS.${NAME} := $(_CFLAGS.${NAME})
endif
ifeq ($${TYPE},header)
_PROVIDES_CXXFLAGS.${NAME} = $(_CXXFLAGS.${NAME})
_PROVIDES_CFLAGS.${NAME} = $(_CFLAGS.${NAME})
endif
_PROVIDES_CPPFLAGS.${NAME} := \
	$(foreach u,${_USES.$(NAME)},$${_PROVIDES_CPPFLAGS.$u}) \
	$(if ${INCLUDEDIR}${INCLUDEDIR.$(TARGET_OS)}${INCLUDEDIR.$(TARGET_ARCH)}${INCLUDEDIR.$(TARGET_OS).$(TARGET_ARCH)}, \
	$(sort -I${CURRENT_PROJECT_DIR} $(addprefix -I${CURRENT_PROJECT_DIR}, \
	${INCLUDEDIR} ${INCLUDEDIR.$(TARGET_OS)} ${INCLUDEDIR.$(TARGET_ARCH)} ${INCLUDEDIR.$(TARGET_OS).$(TARGET_ARCH)})), \
	-I${CURRENT_PROJECT_DIR})
$(eval _SRCS.${NAME} := $(strip $(wildcard $(addprefix ${CURRENT_PROJECT_DIR}, ${SRCS} ${SRCS.$(TARGET_OS)} ${SRCS.$(TARGET_ARCH)} ${SRCS.$(TARGET_OS).$(TARGET_ARCH)}))))
_EXTRA.${NAME} := ${EXTRA} ${EXTRA.$(TARGET_OS)} ${EXTRA.$(TARGET_ARCH)} ${EXTRA.$(TARGET_OS).$(TARGET_ARCH)}
$(if $(_SRCS.${NAME}),,$(error No SRCS found!))
$(call set-sources,${NAME},${_SRCS.$(NAME)})
$(eval _REAL_OBJS.${NAME} := ${OBJS} $(foreach X,${EXTENSIONS},$(patsubst %.$X,$(BUILDDIR)%.o,$(filter %.$X,${_SRCS.$(NAME)}))))
ALL_DEPS := ${ALL_DEPS} $(patsubst %.o,%.d,${_REAL_OBJS.$(NAME)})
$(eval ALL_DIRS := ${ALL_DIRS} $(dir ${_EXEC.$(NAME)} ${_REAL_OBJS.$(NAME)}))
BUILD_DIRS := $$(sort $$(BUILD_DIRS) $$(dir $$(_REAL_OBJS.${NAME})))
CURRENT_PROJECT_DIR := # empty
endef

define copy-rule
$(eval ${TARGETDIR.${_TYPE.$1}}$(notdir $2) :: $2 ; $$(copy))
endef

define add-target
$(info Found ${_EXEC_BASE.$1} ... TYPE=${_TYPE.$1} $(if ${_USES.$1},USES=${_USES.$1}))
ifneq (${_TYPE.$1},header)
all :: ${_EXEC.$1}
test-$1 : ${_EXEC.$1}
test :: test-$1
${_EXEC.$1} : ${_REAL_OBJS.$1} $(foreach u,${_USES.$1},${_EXEC.$u}) | $(dir ${_EXEC.$1}) \
	$(foreach u,${_USES.$1},$(addprefix ${TARGETDIR.${_TYPE.$1}},$(notdir ${_COPYFILES.$u})))
	$$(link.${_TYPE.$1})
${_EXEC.$1} : CXX_MODE := $(if $(foreach u,${_USES.$1},${_SRCS_cpp.$u}),1,)
${_EXEC.$1} : CFLAGS = ${_CFLAGS.$1} $(foreach u,${_USES.$1},${_PROVIDES_CFLAGS.$u})
${_EXEC.$1} : CXXFLAGS = ${_CXXFLAGS.$1} $(foreach u,${_USES.$1},${_PROVIDES_CXXFLAGS.$u})
${_EXEC.$1} : CPPFLAGS = ${_CPPFLAGS.$1} -I${_BASEDIR.$1} $(foreach u,${_USES.$1},${_PROVIDES_CPPFLAGS.$u})
${_EXEC.$1} : LDFLAGS = ${_LDFLAGS.$1} $(foreach u,${_USES.$1},${_PROVIDES_LDFLAGS.$u})
${_EXEC.$1} : LDLIBS = ${_LDLIBS.$1} $(foreach u,${_USES.$1},${_PROVIDES_LDLIBS.$u})
${_EXEC.$1} : ASFLAGS = ${_ASFLAGS.$1}
${_EXEC.$1} : LINKERFILE = ${_LINKERFILE.$1}
${_REAL_OBJS.$1} : | $(addprefix ${_BASEDIR.$1},${_EXTRA.$1})
$$(foreach u,${_USES.$1},$$(foreach f,$${_COPYFILES.$$u},$$(call copy-rule,$1,$$f )))
clean-bins :: ; $$(RM) ${_EXEC.$1} $(foreach u,${_USES.$1},$(addprefix ${TARGETDIR.${_TYPE.$1}},$(notdir ${_COPYFILES.$u})))
ifneq (${_REAL_OBJS.$1},)
clean :: ; $$(RM) ${_REAL_OBJS.$1}
endif
endif
endef

define clean.cmd
	$(RM) $(ALL_DEPS)
	-$(call rmdir,$(wildcard $(call reverse,$(call subby,$(sort ${ALL_DIRS})))))
endef

# Create missing directories
%/ : ; @echo Creating $@ ; $(MKDIR) $@

.PRECIOUS : %/

# generate rules
.SECONDEXPANSION :
$(foreach X,${EXTENSIONS},$(eval $(BUILDDIR)%.o : %.$X | $$$$(@D)/ ; $$(compile.$X)))

# collect all projects
PROJECTS := $(call rwildcard,$(PROJECT_DIRS),*.prj)

# save any parameters to apply later to all projects
GLOBAL_CFLAGS := $(CFLAGS)
GLOBAL_CXXFLAGS := $(CXXFLAGS)
GLOBAL_CPPFLAGS := $(CPPFLAGS)
GLOBAL_LDFLAGS := $(LDFLAGS)
GLOBAL_LDLIBS := $(LDLIBS)
GLOBAL_ASFLAGS := $(ASFLAGS)

$(info BUILDDIR=${BUILDDIR})
$(info OS=${OS} ARCH=${ARCH})

# Each project file must wrap beginning and end of project sections with these
BEGIN_TARGET = $(eval $(call clear-vars) $(call begin-project))
END_TARGET = $(eval $(call store-project))

include $(PROJECTS)

# $(info TARGET_LIST = ${TARGET_LIST})
$(eval $(call clear-vars))
$(foreach T,${TARGET_LIST},$(eval $(call add-target,$T)))

ALL_DEPS := $(sort ${ALL_DEPS})
-include ${ALL_DEPS}
