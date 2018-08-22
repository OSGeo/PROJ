#!/bin/sh
EMSCRIPTEN="/emsdk_portable/sdk"

if [ ! -x "${EMSCRIPTEN}/emmake" ] ; then
	echo "No emscripten SDK found under ${EMSCRIPTEN}, are you sure you're running this from within the docker container shell?"
	echo
	echo "e.g.: docker run -it -v $(pwd):/src trzeci/emscripten:sdk-incoming-64bit bash"
	echo
	exit 1
fi

WASM_STAGING_DIR=$(pwd)/build-wasm
JS_STAGING_DIR=$(pwd)/build-js

echo "Staging WASM build under: ${WASM_STAGING_DIR} ..."
mkdir -p ${WASM_STAGING_DIR} && cd ${WASM_STAGING_DIR} \
 && cmake .. -DCMAKE_TOOLCHAIN_FILE="${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_BUILD_TYPE=Release -DWASM=1 && \
 ${EMSCRIPTEN}/emmake make VERBOSE=1 && \
 cd ..


echo "Staging JS build under: ${JS_STAGING_DIR} ..."
mkdir -p ${JS_STAGING_DIR} && cd ${JS_STAGING_DIR} \
  && pwd \
 && cmake .. -DCMAKE_TOOLCHAIN_FILE="${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_BUILD_TYPE=Release && \
 ${EMSCRIPTEN}/emmake make VERBOSE=1 && \
 cd ..
