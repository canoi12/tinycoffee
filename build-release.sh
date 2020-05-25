#!/bin/bash

platform=$1
CC=c99-gcc
FILES=release.c
out=tico
INCLUDE=-Isrc/external/\ -Isrc\ -Isrc/external/freetype/include
LIBS=""
CFLAGS=
FLAGS=-lglfw\ -lgl3w\ -lm\ -lpthread\ -lX11\ -ldl\ -lGL
compile_wren=true
compile_lua=false
development=true

EXTERNAL=src/external
GLFW_SOURCE=$EXTERNAL/glfw/src

DEFINE=-D_GLFW_X11

if [[ "$platform" != "windows" ]]; then
  platform="linux"
fi

echo "compiling for $platform..."

if [[ $platform == "windows" ]]; then
  CC=x86_64-w64-mingw32-gcc
  FLAGS=-lmingw32\ -lgl3w\ -lglfw\ -lopengl32\ -lgdi32\ -lwinmm
  out=tico.exe
  DEFINE=-D_GLFW_WIN32
fi

FOLDER=builds/$platform
out="bin/$platform/$out"

if [ ! -d "$FOLDER" ]; then
  $(mkdir -p $FOLDER)
fi

if [ ! -d "bin/$platform" ]; then
  mkdir -p "bin/$platform"
fi

if [ -f "$out" ]; then
  echo "clearing executable"
  (rm "$out")
fi

if [ ! -d "$FOLDER/libs" ]; then
  echo "creating libs folder"
  $(mkdir $FOLDER/libs)
fi

# if [ ! -f "$FOLDER/libs/libminiz.a" ]; then
#   echo "compiling miniz static lib.."
#   $($CC -c $EXTERNAL/miniz/miniz.c -o $FOLDER/miniz.o $INCLUDE)

#   $(ar rcs $FOLDER/libs/libminiz.a $FOLDER/miniz.o)
#   $(rm $FOLDER/miniz.o)
#   FLAGS="-lminiz $FLAGS"
# else
#   FLAGS="-lminiz $FLAGS"
# fi
if [ ! -f "$FOLDER/libs/libzip.a" ]; then
  echo "compiling zip static lib.."
  $($CC -c $EXTERNAL/zip/src/zip.c -o $FOLDER/zip.o $INCLUDE)

  $(ar rcs $FOLDER/libs/libzip.a $FOLDER/zip.o)
  $(rm $FOLDER/zip.o)
  FLAGS="-lzip $FLAGS"
else
  FLAGS="-lzip $FLAGS"
fi

if [[ ! -f "$FOLDER/libs/libglfw.a" ]]; then
  echo "compiling glfw static lib .."
  if [ -d "$FOLDER/glfw" ]; then
    $(rm -r $FOLDER/glfw)
  fi
  if [[ $platform == "windows" ]]; then
    $(mkdir $FOLDER/glfw)

    $($CC -c $GLFW_SOURCE/init.c -o $FOLDER/glfw/init.o $DEFINE)
    $($CC -c $GLFW_SOURCE/context.c -o $FOLDER/glfw/context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/input.c -o $FOLDER/glfw/input.o $DEFINE)
    $($CC -c $GLFW_SOURCE/monitor.c -o $FOLDER/glfw/monitor.o $DEFINE)
    $($CC -c $GLFW_SOURCE/window.c -o $FOLDER/glfw/window.o $DEFINE)
    $($CC -c $GLFW_SOURCE/win32_init.c -o $FOLDER/glfw/x11_init.o $DEFINE)
    $($CC -c $GLFW_SOURCE/win32_monitor.c -o $FOLDER/glfw/x11_monitor.o $DEFINE)
    $($CC -c $GLFW_SOURCE/win32_window.c -o $FOLDER/glfw/x11_window.o $DEFINE)
    # $($CC -c $GLFW_SOURCE/xkb_unicode.c -o $FOLDER/glfw/xkb_unicode.o $DEFINE)
    $($CC -c $GLFW_SOURCE/win32_thread.c -o $FOLDER/glfw/posix_thread.o $DEFINE)
    $($CC -c $GLFW_SOURCE/win32_time.c -o $FOLDER/glfw/posix_time.o $DEFINE)
    $($CC -c $GLFW_SOURCE/wgl_context.c -o $FOLDER/glfw/wgl_context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/egl_context.c -o $FOLDER/glfw/egl_context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/osmesa_context.c -o $FOLDER/glfw/osmesa_context.o $DEFINE)
    # $($CC -c $GLFW_SOURCE/glx_context.c -o $FOLDER/glfw/glx_context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/win32_joystick.c -o $FOLDER/glfw/linux_joystick.o $DEFINE)
    $($CC -c $GLFW_SOURCE/vulkan.c -o $FOLDER/glfw/vulkan.o $DEFINE)
    $(ar rcs $FOLDER/libs/libglfw.a $FOLDER/glfw/*.o)
    $(rm -r $FOLDER/glfw)
  else
    $(mkdir $FOLDER/glfw)

    $($CC -c $GLFW_SOURCE/init.c -o $FOLDER/glfw/init.o $DEFINE)
    $($CC -c $GLFW_SOURCE/context.c -o $FOLDER/glfw/context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/input.c -o $FOLDER/glfw/input.o $DEFINE)
    $($CC -c $GLFW_SOURCE/monitor.c -o $FOLDER/glfw/monitor.o $DEFINE)
    $($CC -c $GLFW_SOURCE/window.c -o $FOLDER/glfw/window.o $DEFINE)
    $($CC -c $GLFW_SOURCE/x11_init.c -o $FOLDER/glfw/x11_init.o $DEFINE)
    $($CC -c $GLFW_SOURCE/x11_monitor.c -o $FOLDER/glfw/x11_monitor.o $DEFINE)
    $($CC -c $GLFW_SOURCE/x11_window.c -o $FOLDER/glfw/x11_window.o $DEFINE)
    $($CC -c $GLFW_SOURCE/xkb_unicode.c -o $FOLDER/glfw/xkb_unicode.o $DEFINE)
    $($CC -c $GLFW_SOURCE/posix_thread.c -o $FOLDER/glfw/posix_thread.o $DEFINE)
    $($CC -c $GLFW_SOURCE/posix_time.c -o $FOLDER/glfw/posix_time.o $DEFINE)
    $($CC -c $GLFW_SOURCE/egl_context.c -o $FOLDER/glfw/egl_context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/osmesa_context.c -o $FOLDER/glfw/osmesa_context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/glx_context.c -o $FOLDER/glfw/glx_context.o $DEFINE)
    $($CC -c $GLFW_SOURCE/linux_joystick.c -o $FOLDER/glfw/linux_joystick.o $DEFINE)
    $($CC -c $GLFW_SOURCE/vulkan.c -o $FOLDER/glfw/vulkan.o $DEFINE)

    $(ar rcs $FOLDER/libs/libglfw.a $FOLDER/glfw/*.o)
    $(rm -r $FOLDER/glfw)
  fi
fi

if [ ! -f "$FOLDER/libs/libgl3w.a" ]; then
  echo "compiling gl3w static lib.."
  $($CC -c $EXTERNAL/GL/gl3w.c -o $FOLDER/gl3w.o $INCLUDE)

  $(ar rcs $FOLDER/libs/libgl3w.a $FOLDER/gl3w.o)
  $(rm $FOLDER/gl3w.o)
fi

if [ ! -f "$FOLDER/libs/libfreetype.a" ]; then
  echo "compiling freetype static lib.."
  if [ ! -d "$FOLDER/freetype" ]; then
    $(mkdir $FOLDER/freetype)
  fi
  $($CC -c $EXTERNAL/freetype/src/base/ftsystem.c -o $FOLDER/freetype/ftsystem.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/base/ftinit.c -o $FOLDER/freetype/ftinit.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/base/ftdebug.c -o $FOLDER/freetype/ftdebug.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/base/ftbase.c -o $FOLDER/freetype/ftbase.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/base/ftbbox.c -o $FOLDER/freetype/ftbbox.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/autofit/autofit.c -o $FOLDER/freetype/autofit.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)

  $($CC -c $EXTERNAL/freetype/src/base/ftglyph.c -o $FOLDER/freetype/ftglyph.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/base/ftbitmap.c -o $FOLDER/freetype/ftbitmap.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)

  $($CC -c $EXTERNAL/freetype/src/truetype/truetype.c -o $FOLDER/freetype/truetype.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/sfnt/sfnt.c -o $FOLDER/freetype/sfnt.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/psnames/psnames.c -o $FOLDER/freetype/psnames.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/bdf/bdf.c -o $FOLDER/freetype/bdf.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/cff/cff.c -o $FOLDER/freetype/cff.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/cid/type1cid.c -o $FOLDER/freetype/type1cid.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/pcf/pcf.c -o $FOLDER/freetype/pcf.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/pfr/pfr.c -o $FOLDER/freetype/pfr.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/type1/type1.c -o $FOLDER/freetype/type1.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/type42/type42.c -o $FOLDER/freetype/type42.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/winfonts/winfnt.c -o $FOLDER/freetype/winfnt.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/psaux/psaux.c -o $FOLDER/freetype/psaux.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/pshinter/pshinter.c -o $FOLDER/freetype/pshinter.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/smooth/smooth.c -o $FOLDER/freetype/smooth.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)

  $($CC -c $EXTERNAL/freetype/src/gzip/ftgzip.c -o $FOLDER/freetype/ftgzip.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  $($CC -c $EXTERNAL/freetype/src/lzw/ftlzw.c -o $FOLDER/freetype/ftlzw.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)

  $($CC -c $EXTERNAL/freetype/src/raster/raster.c -o $FOLDER/freetype/raster.o $INCLUDE -DFT2_BUILD_LIBRARY $CFLAGS)
  # $($CC $EXTERNAL/freetype/src/*/*.c -o $FOLDER/freetype.o $INCLUDE $CFLAGS)

  $(ar rcs $FOLDER/libs/libfreetype.a $FOLDER/freetype/*.o)
  # $(rm $FOLDER/freetype.o)
  $(rm -r $FOLDER/freetype)
  FLAGS="-lfreetype $FLAGS"
else
  FLAGS="-lfreetype $FLAGS"
  # echo "skip"
fi

if $compile_wren; then
  if [ ! -f "$FOLDER/libs/libwren.a" ]; then
    echo "compiling wren.."
    INCLUDE="$INCLUDE -I$EXTERNAL/wren/src/include -I$EXTERNAL/wren/src/vm -I$EXTERNAL/wren/src/optional"
    wren_dir="$EXTERNAL/wren/src"
    if [ ! -d "$FOLDER/wren" ]; then
      $(mkdir $FOLDER/wren)
    fi
    $($CC -c $wren_dir/vm/wren_compiler.c -o $FOLDER/wren/compiler.o $INCLUDE)
    $($CC -c $wren_dir/vm/wren_core.c -o $FOLDER/wren/core.o $INCLUDE)
    $($CC -c $wren_dir/vm/wren_debug.c -o $FOLDER/wren/debug.o $INCLUDE)
    $($CC -c $wren_dir/vm/wren_primitive.c -o $FOLDER/wren/primitive.o $INCLUDE)
    $($CC -c $wren_dir/vm/wren_utils.c -o $FOLDER/wren/utils.o $INCLUDE)
    $($CC -c $wren_dir/vm/wren_value.c -o $FOLDER/wren/value.o $INCLUDE)
    $($CC -c $wren_dir/vm/wren_vm.c -o $FOLDER/wren/vm.o $INCLUDE)

    $($CC -c $wren_dir/optional/wren_opt_meta.c -o $FOLDER/wren/opt_meta.o $INCLUDE)
    $($CC -c $wren_dir/optional/wren_opt_random.c -o $FOLDER/wren/opt_random.o $INCLUDE)

    $(ar rcs $FOLDER/libs/libwren.a $FOLDER/wren/*.o)
    $(rm -r $FOLDER/wren)
  fi

  DEFINE="$DEFINE -DWREN_LANG"
  FLAGS="$FLAGS -lwren"

fi

if [ ! -f "$FOLDER/libs/libtico.a" ] || $development; then
  echo "compiling tico static lib.."
  if [ ! -d "$FOLDER/tico" ]; then
    $(mkdir $FOLDER/tico)
  fi
  $($CC -c src/core.c -o $FOLDER/tico/core.o $INCLUDE $DEFINE)
  $($CC -c src/texture.c -o $FOLDER/tico/texture.o $INCLUDE $DEFINE)
  $($CC -c src/tcwren.c -o $FOLDER/tico/tcwren.o $INCLUDE $DEFINE)
  # $($CC -c src/tclua.c -o $FOLDER/tico/tclua.o)

  $(ar rcs $FOLDER/libs/libtico.a $FOLDER/tico/*.o)
  $(rm -r $FOLDER/tico)
  FLAGS="-ltico $FLAGS"
else
  FLAGS="-ltico $FLAGS"
  # echo "skip"
fi

echo "compiling executable.."
CMD=$CC\ $FILES\ -o\ $out\ $CFLAGS\ -L$FOLDER/libs\ $LIBS\ $INCLUDE\ $FLAGS
echo "$CMD"
$($CMD)

if [ ! -f "$out" ]; then
  echo "build failed.."
else
  echo "build successful"
fi
