CC = gcc
CROSS =
PLATFORM := Linux
MODE := Debug

EXTERNAL = src/external
TMP = temp

DEFINE =
CFLAGS = -g -std=gnu99
LFLAGS = -lX11 -lm -ldl -lpthread -lGL
LIBNAME = libtico
SLIBNAME = $(LIBNAME).a
DLIBNAME = $(LIBNAME).so
BINNAME = tico


INCLUDE = -I$(EXTERNAL) -Isrc

DEFINE += -DWREN_LANG
INCLUDE += -I$(EXTERNAL)/wren/src/include -I$(EXTERNAL)/wren/src/optional -I$(EXTERNAL)/wren/src/vm

DJITOBJ = $(EXTERNAL)/luajit/src/*_dyn.o
SJITOBJ = $(EXTERNAL)/luajit/src/*.o

GLFW_FOLDER := $(EXTERNAL)/glfw/src
LUA_FOLDER := $(EXTERNAL)/lua/src
TICO_FOLDER := src
ZIP_FOLDER := $(EXTERNAL)/zip/src
CJSON_FOLDER := $(EXTERNAL)/cjson
GL3W_FOLDER := $(EXTERNAL)/GL
WREN_FOLDER := $(EXTERNAL)/wren/src

GLFW_SOURCES = init.c context.c input.c monitor.c window.c egl_context.c osmesa_context.c vulkan.c
LUA_SOURCES = $(wildcard $(LUA_FOLDER)/*.c)
# TICO_SOURCES = $(wildcard $(TICO_FOLDER)/*.c $(TICO_FOLDER)/ui/*.c)
TICO_SOURCES = $(wildcard $(TICO_FOLDER)/*.c $(TICO_FOLDER)/ui/microui.c)
WREN_SOURCES = $(wildcard $(WREN_FOLDER)/vm/*.c $(WREN_FOLDER)/optional/*.c)

TARGET_OS = Linux
OUT = bin/$(BINNAME)

ifeq ($(PLATFORM), Windows)
	CROSS = x86_64-w64-mingw32-
	LFLAGS = -lmingw32 -lopengl32 -lgdi32 -lwinmm -mwindows
	OUT = bin/$(BINNAME).exe
	TARGET_OS = Windows
	DLIBNAME = $(LIBNAME).dll
	DEFINE += -D_GLFW_WIN32

	DJITOBJ = $(SJITOBJ)

	GLFW_SOURCES += win32_init.c win32_monitor.c win32_window.c win32_thread.c win32_time.c win32_joystick.c wgl_context.c
else ifeq ($(PLATFORM), OSX)
	DLIBNAME = $(LIBNAME).dylib
else
	DEFINE += -D_GLFW_X11
	GLFW_SOURCES += x11_init.c x11_monitor.c x11_window.c xkb_unicode.c posix_thread.c posix_time.c linux_joystick.c glx_context.c
endif

ifeq ($(MODE), Release)
	CFLAGS += -O2
endif

GLFW_OBJ = $(GLFW_SOURCES:%.c=$(GLFW_FOLDER)/%.o)
LUA_OBJ = $(LUA_SOURCES:%.c=%.o)
TICO_OBJ = $(TICO_SOURCES:%.c=%.o)
WREN_OBJ = $(WREN_SOURCES:%.c=%.o)
ZIP_OBJ = $(ZIP_FOLDER)/zip.o
CJSON_OBJ = $(CJSON_FOLDER)/cJSON.o
GL3W_OBJ = $(GL3W_FOLDER)/gl3w.o

TOBJ += $(GLFW_OBJ) $(TICO_OBJ) $(ZIP_OBJ) $(CJSON_OBJ) $(GL3W_OBJ) $(WREN_OBJ)

ifdef NOJIT
	TOBJ += $(LUA_OBJ)
else
  DEFINE += -DLUAJIT -DLUAMOD_API=
  COMPILE_JIT = luajit
endif

SOURCES = $(TOBJ:%.o=%.c)
OBJ = $(subst /,__, $(TOBJ))
SOBJ = $(OBJ:%.o=$(TMP)/static/%.o)
DOBJ = $(OBJ:%.o=$(TMP)/shared/%.o)

LUA_SCRIPTS = $(wildcard src/scripting/lua/*.lua)
LUAH_SCRIPTS = $(LUA_SCRIPTS:%.lua=%.lua.h)
LUAH_FILES = $(wildcard src/scripting/lua/*.h)

WREN_SCRIPTS = $(wildcard src/scripting/wren/*.wren)
WRENH_SCRIPTS = $(WREN_SCRIPTS:%.wren=%.wren.h)
WRENH_FILES = $(wildcard src/scripting/wren/*.h)

GLSL_FILES = $(wildcard src/shaders/*.glsl src/shaders/*.frag src/shaders/*.vert)
GLSLH_FILES = $(GLSL_FILES:%=%.h)


hello:
	@echo "Hello"

embed:
	gcc cembed.c -o embed

.SECONDEXPANSION:
$(TMP)/static/%.o: $$(subst __,/,%.c)
#	$(eval SRC = $(shell echo $< | sed -e 's/__/\//g;'))
	$(CROSS)$(CC) -c $< -o $@ $(DEFINE) $(INCLUDE) $(CFLAGS)

$(TMP)/shared/%.o: $$(subst __,/,%.c)
	$(CROSS)$(CC) -c $< -o $@ -fPIC $(DEFINE) -D_GLFW_BUILD_DLL -DBUILD_SHARED $(INCLUDE) $(CFLAGS)

%.lua.h: %.lua embed
	./embed $<

%.wren.h: %.wren embed
	./embed $<

%.glsl.h: %.glsl embed
	./embed $<

%.frag.h: %.frag embed
	./embed $<

%.vert.h: %.vert embed
	./embed $<


setup:
	if [ ! -d $(TMP) ]; then \
		mkdir -p $(TMP)/static; \
		mkdir -p $(TMP)/shared; \
	fi; \
	if [ ! -d bin/ ]; then \
		mkdir bin; \
	fi

luajit:
	cd $(EXTERNAL)/luajit/src && $(MAKE) HOST_CC=$(CC) CROSS=$(CROSS) TARGET_SYS=$(PLATFORM)
	cp $(filter-out %_dyn.o,$(shell echo $(SJITOBJ))) $(TMP)/static
	cp $(DJITOBJ) $(TMP)/shared
	rm -f $(TMP)/shared/luajit.o

$(SLIBNAME): setup $(LUAH_SCRIPTS) $(GLSLH_FILES) $(WRENH_SCRIPTS) $(SOBJ) $(COMPILE_JIT) $(LUAH_FILES)
	ar rcs $(LIBNAME).a $(TMP)/static/*.o

$(DLIBNAME): setup $(LUAH_SCRIPTS) $(GLSLH_FILES) $(WRENH_SCRIPTS) $(DOBJ) $(COMPILE_JIT) $(LUAH_FILES)
	$(CROSS)$(CC) -shared -o $@ $(TMP)/shared/*.o -DBUILD_SHARED $(CFLAGS) $(DEFINE) $(LFLAGS) $(INCLUDE)

tico: $(SLIBNAME)
	$(CROSS)$(CC) main.c -o $(OUT) -L. -Wl,-Bstatic -ltico -Wl,-Bdynamic $(DEFINE) $(LFLAGS) $(CFLAGS) $(INCLUDE)
	strip $(OUT)

all: tico $(DLIBNAME)
	rm -r $(TMP)

clean_builds:
	rm -rf $(TMP)
	rm -f $(SLIBNAME)
	rm -f $(DLIBNAME)
	rm -f src/scripting/lua/*.lua.h
	rm -f src/shaders/*.h
	cd $(EXTERNAL)/luajit/src && $(MAKE) clean

clean: clean_builds
	rm -rf bin
