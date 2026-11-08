#!/bin/sh
# Builds fontconvert.js: the unmodified ../fontconvert.c compiled to
# WebAssembly with Emscripten's FreeType port. The .wasm is embedded in the
# .js (SINGLE_FILE) so the page also works when opened from disk.
# Needs emcc (https://emscripten.org). Run from any directory.
set -e
cd "$(dirname "$0")/.."
emcc -O2 -Wall -sUSE_FREETYPE=1 fontconvert.c -o web/fontconvert.js \
  -sMODULARIZE=1 -sEXPORT_NAME=createFontconvert \
  -sINVOKE_RUN=0 -sEXIT_RUNTIME=1 \
  -sEXPORTED_RUNTIME_METHODS=callMain,FS -sFORCE_FILESYSTEM=1 \
  -sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web,node -sSINGLE_FILE=1
emcc --version | head -1
