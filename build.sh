#!/bin/bash

cc=gcc
out=tico
sources=main.c
release="debug"
include=-Isrc\ -Isrc/external\ -Isrc/external/freetype/include
cflags=-g\ --std=gnu99
lflags=-lX11\ -lm\ -ldl\ -lGL\ -lpthread
wren_lang=true
lua_lang=false
development=true
error=false

external=src/external
glfw_folder="$external/glfw/src"

define=-D_GLFW_X11

config_windows()
{
  cc=x86_64-w64-mingw32-gcc
  out="$out.exe"
  platform="windows"
  lflags="-lmingw32 -lopengl32 -lgdi32 -lwinmm -mwindows"
  define=-D_GLFW_WIN32
}

clear_folders()
{
  echo "cleaning up folders.."
  rm -r bin/
  rm -r builds/
}

for arg in "$@"
do
  if [[ $arg == "windows" ]]; then
    config_windows
  fi
  if [[ $arg == "release" ]]; then
    cflags="$cflags -O3"
    release="release"
    sources="release.c"
  fi
  if [[ $arg == "clear" ]]; then
    clear_folders
  fi
done

if [[ $platform == "" ]]; then
  platform="linux"
fi

echo "compiling $platform $release.."

folder="builds/$release/$platform"
bin_folder="bin/$release/$platform"
libs_folder="$folder/libs"
objs_folder="$folder/objs"
out="$bin_folder/$out"

if [ ! -d $folder ]; then
  echo "creating builds folder.."
  mkdir -p "$folder"
fi

if [ ! -d "bin/$release/$platform"  ]; then
  echo "creating bin folder.."
  mkdir -p "bin/$release/$platform"
fi

if [ -f $out ]; then
  echo "cleaning up exec $out.."
  rm "$out"
fi

if [ ! -d "$libs_folder" ]; then
  echo "creating libs folder"
  mkdir -p "$libs_folder"
  mkdir -p "$objs_folder"
fi

if [ ! -f "$libs_folder/libzip" ]; then
  echo "compiling zip static lib.."
  $cc -c $external/zip/src/zip.c -o $objs_folder/zip.o $cflags

  touch $libs_folder/libzip
  # ar rcs $libs_folder/libzip.a $objs_folder/zip.o
  # rm $objs_folder/zip.o
fi

if [[ ! -f "$libs_folder/libglfw" ]]; then
  echo "compiling glfw static lib .."
  # if [ -d "$FOLDER/glfw" ]; then
  #   $(rm -r $FOLDER/glfw)
  # fi
  glfw_sources="init.c context.c input.c monitor.c window.c egl_context.c osmesa_context.c vulkan.c"
  if [[ $platform == "windows" ]]; then
    glfw_sources="$glfw_sources win32_init.c win32_monitor.c win32_window.c win32_thread.c win32_time.c win32_joystick.c wgl_context.c"
  else
    glfw_sources="$glfw_sources x11_init.c x11_monitor.c x11_window.c xkb_unicode.c posix_thread.c posix_time.c linux_joystick.c glx_context.c"
  fi

  for glfwsrc in $glfw_sources; do
    glfwobj=$(echo "$glfwsrc" | sed -e 's/\.c/.o/g')
    $cc -c "$glfw_folder/$glfwsrc" -o "$objs_folder/$glfwobj" $cflags $define
    if [[ $? -ne 0 ]]; then
      echo "failed to compile $glfwsrc"
    fi
  done

  touch $libs_folder/libglfw


  # $(ar rcs $libs_folder/libglfw.a $objs_folder/*.o)
  # rm $objs_folder/*
fi

if [ ! -f "$libs_folder/libgl3w" ]; then
  echo "compiling gl3w static lib.."
  $cc -c $external/GL/gl3w.c -o $objs_folder/gl3w.o $cflags $include

  touch $libs_folder/libgl3w
  # $(ar rcs $libs_folder/libgl3w.a $objs_folder/gl3w.o)
  # rm $objs_folder/gl3w.o
fi

# if [ ! -f "$libs_folder/libfreetype" ]; then
#   echo "compiling freetype static lib.."
#   freetype_dir=$external/freetype/src
#   freetype_sources="base/ftsystem.c base/ftinit.c base/ftdebug.c base/ftbase.c base/ftbbox.c autofit/autofit.c\
#   base/ftglyph.c base/ftbitmap.c truetype/truetype.c sfnt/sfnt.c psnames/psnames.c bdf/bdf.c cff/cff.c cid/type1cid.c\
#   pcf/pcf.c pfr/pfr.c type1/type1.c type42/type42.c winfonts/winfnt.c psaux/psaux.c pshinter/pshinter.c smooth/smooth.c\
#   gzip/ftgzip.c lzw/ftlzw.c raster/raster.c"

#   for ftsrc in $freetype_sources; do
#     ftobj=$(echo $ftsrc | sed -e 's/\//_/g' | sed -e 's/\.c/.o/g')
#     $cc -c $freetype_dir/$ftsrc -o $objs_folder/$ftobj $include -DFT2_BUILD_LIBRARY $cflags
#   done

#   touch $libs_folder/libfreetype

#   # $(ar rcs $libs_folder/libfreetype.a $objs_folder/*.o)
#   # $(rm -r $objs_folder/*.o)
# fi

if $wren_lang; then
  if [ ! -f "$libs_folder/libwren" ]; then
    echo "compiling wren.."
    include="$include -I$external/wren/src/include -I$external/wren/src/vm -I$external/wren/src/optional"
    wren_dir="$external/wren/src"

    for wrensrc in $(ls $wren_dir/vm/ | grep '\.c'); do
      wrenobj=$(echo "$wrensrc" | sed -e 's/\.c/.o/g')
      $cc -c $wren_dir/vm/$wrensrc -o $objs_folder/$wrenobj $include $cflags
    done

    for wrensrc in $(ls $wren_dir/optional/ | grep '\.c'); do
      wrenobj=$(echo "$wrensrc" | sed -e 's/\.c/.o/g')
      $cc -c $wren_dir/optional/$wrensrc -o $objs_folder/$wrenobj $include $cflags
    done

    touch $libs_folder/libwren

    # $(ar rcs $FOLDER/libs/libwren.a $FOLDER/wren/*.o)
    # $(rm -r $FOLDER/wren)
  fi

  define="$define -DWREN_LANG"
  # lflags="$lflags -lwren"
fi

if [ ! -f "$libs_folder/libtico.a" ] || $development; then
  echo "compiling tico static lib.."
  $cc -c src/core.c -o $objs_folder/tccore.o $include $define $cflags
  $cc -c src/texture.c -o $objs_folder/tctexture.o $include $define $cflags
  $cc -c src/tcwren.c -o $objs_folder/tcwren.o $include $define $cflags
  # $($CC -c src/tclua.c -o $FOLDER/tico/tclua.o)

  $(ar rcs $libs_folder/libtico.a $objs_folder/*.o)
  # $(rm -r $FOLDER/tico)
  lflags="-ltico $lflags"
else
  lflags="-ltico $lflags"
  # echo "skip"
fi

lflags="-L$libs_folder $lflags"

echo "compiling executable.."
cmd="$cc $sources -o $out $cflags $lflags $include $cflags"
echo "$cmd"

$($cmd)

if [ $? -eq 1 ]; then
  echo "build failed.."
else
  echo "build successful.."
fi