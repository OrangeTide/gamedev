#!/bin/bash
set -xe

# drSoft MiniAudio
curl -L -O https://raw.githubusercontent.com/dr-soft/mini_al/master/miniaudio.h

# STB
curl -L \
  -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h \
  -O https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h \
  -O https://raw.githubusercontent.com/nothings/stb/master/stb_ds.h \
  -O https://raw.githubusercontent.com/nothings/stb/master/LICENSE

# git_load
curl -L -O https://raw.githubusercontent.com/hidefromkgb/gif_load/master/gif_load.h

# zip_load
curl -L -O https://raw.githubusercontent.com/hidefromkgb/zip_load/master/zip_load.h
