#!/bin/sh
set -xe
# OpenGL ES 2.0
GLES2_HEADERS=("https://registry.khronos.org/OpenGL/api/GLES2/gl2.h" "https://registry.khronos.org/OpenGL/api/GLES2/gl2ext.h" "https://registry.khronos.org/OpenGL/api/GLES2/gl2platform.h")
mkdir -p include/GLES2
cd include/GLES2
for f in "${GLES2_HEADERS[@]}" ; do
  curl -O $f
done
cd -
# OpenGL ES 3.0, 3.1, and 3.2
GLES3_HEADERS=("https://registry.khronos.org/OpenGL/api/GLES3/gl32.h" "https://registry.khronos.org/OpenGL/api/GLES3/gl31.h" "https://registry.khronos.org/OpenGL/api/GLES3/gl3.h" "https://registry.khronos.org/OpenGL/api/GLES3/gl3platform.h")
mkdir -p include/GLES3
cd include/GLES3
for f in "${GLES2_HEADERS[@]}" ; do
  curl -O $f
done
cd -
