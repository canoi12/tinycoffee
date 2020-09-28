cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set(LIBS GL)

if (WIN32)
	set(CMAKE_SYSTEM_NAME Windows)
	set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
	# set(CMAKE_C_COMPILER /usr/bin/${TOOLCHAIN_PREFIX}-gcc)
	# set(CMAKE_CXX_COMPILER /usr/bin/${TOOLCHAIN_PREFIX}-g++)
	SET(CMAKE_C_COMPILER     "x86_64-w64-mingw32-gcc")
	SET(CMAKE_CXX_COMPILER   "x86_64-w64-mingw32-g++")
	SET(CMAKE_RC_COMPILER    "x86_64-w64-mingw32-windres")
	SET(CMAKE_RANLIB         "x86_64-w64-mingw32-ranlib")

	# set(MINGW ON)
	message("Compiling for Windows")

	SET(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")
	SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

	set(LIBS mingw32 opengl32 gdi32 winmm -mwindows)
	# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwindows")
endif(WIN32)

project(tico VERSION 0.3.0 LANGUAGES CXX C)

include_directories(include)
include_directories(external)
include_directories(external/glfw/include)

set(cimgui external/cimgui)
set(imgui_impl ${cimgui}/imgui/examples)
set(gl3w external/GL/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

include_directories(${cimgui} ${cimgui}/imgui ${imgui_impl})

file(GLOB cimgui_files 
	"${cimgui}/*.cpp"
	"${cimgui}/imgui/*.cpp"
	"${imgui_impl}/imgui_impl_glfw.cpp"
	"${imgui_impl}/imgui_impl_opengl3.cpp")

add_library(cimgui STATIC ${cimgui_files})

target_compile_definitions(cimgui PRIVATE 
	IMGUI_IMPL_API=extern\ \"C\"
	IMGUI_IMPL_OPENGL_LOADER_GL3W)
if (WIN32)
	target_link_libraries(cimgui -static)
endif(WIN32)

# set(GLFW_BUILD_EXAMPLES OFF)
# set(GLFW_BUILD_TESTS OFF)
# set(GLFW_INSTALL OFF)
# add_subdirectory(external/glfw)
set(GLFW_FOLDER external/glfw)
set(GLFW_DEFS _GLFW_X11)
set(GLFW_SOURCES 
	${GLFW_FOLDER}/src/init.c
	${GLFW_FOLDER}/src/context.c
	${GLFW_FOLDER}/src/input.c
	${GLFW_FOLDER}/src/monitor.c
	${GLFW_FOLDER}/src/window.c
	${GLFW_FOLDER}/src/egl_context.c
	${GLFW_FOLDER}/src/osmesa_context.c
	${GLFW_FOLDER}/src/vulkan.c)

set(GLFW_LIBS)

if (WIN32)
	set(GLFW_SOURCES 
		${GLFW_SOURCES}
		${GLFW_FOLDER}/src/win32_init.c
		${GLFW_FOLDER}/src/win32_monitor.c
		${GLFW_FOLDER}/src/win32_window.c
		${GLFW_FOLDER}/src/win32_thread.c
		${GLFW_FOLDER}/src/win32_time.c
		${GLFW_FOLDER}/src/win32_joystick.c
		${GLFW_FOLDER}/src/wgl_context.c)
	set(GLFW_DEFS _GLFW_WIN32)
	set(GLFW_LIBS ${GLFW_LIBS} ${LIBS} m pthread)
else()
	set(GLFW_SOURCES 
		${GLFW_SOURCES}
		${GLFW_FOLDER}/src/x11_init.c
		${GLFW_FOLDER}/src/x11_monitor.c
		${GLFW_FOLDER}/src/x11_window.c
		${GLFW_FOLDER}/src/xkb_unicode.c
		${GLFW_FOLDER}/src/posix_thread.c
		${GLFW_FOLDER}/src/posix_time.c
		${GLFW_FOLDER}/src/linux_joystick.c
		${GLFW_FOLDER}/src/glx_context.c)
	set(GLFW_LIBS ${GLFW_LIBS} m dl X11 pthread)
endif()

add_library(glfw STATIC ${GLFW_SOURCES})
target_compile_definitions(glfw PRIVATE ${GLFW_DEFS})
target_link_libraries(glfw ${GLFW_LIBS})
# target_compile_options(glfw PRIVATE -static)

# set(LIBS )

file(GLOB tico_sources src/*.c)
file(GLOB lua_sources external/lua/src/*.c)

add_library(lua STATIC ${lua_sources})
add_library(gl3w STATIC ${gl3w}/gl3w.c)
target_link_libraries(gl3w ${LIBS})

add_library(tico_lib STATIC
	${tico_sources} 
	external/map.c
	external/vec.c 
	external/zip/src/zip.c 
	external/cjson/cJSON.c)
target_link_libraries(tico_lib ${LIBS} glfw gl3w lua cimgui)
# target_compile_options(tico_lib PRIVATE -static)
# target_compile_definitions(tico_lib PRIVATE TICO_NO_RENDER_MODULE)
add_executable(tico main.c)
target_link_libraries(tico tico_lib ${LIBS})
target_compile_definitions(tico PRIVATE IMGUI_IMPL_API=\ )

# add_executable(glfw_out glfw.c)
# target_link_libraries(glfw_out tico_lib gl3w ${LIBS})