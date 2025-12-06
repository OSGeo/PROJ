#!/bin/bash

###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Build with emscripten compiler
#  Author:   Javier Jimenez Shaw
#
###############################################################################
#  Copyright (c) 2025 Javier Jimenez Shaw
###############################################################################

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration Variables ---
# Use environment variables from Dockerfile or runtime.
PROJ_REPO=${PROJ_REPO:-https://github.com/OSGeo/PROJ.git}

EMSDK_PATH=${EMSDK_PATH:-/emsdk}
BUILD_DIR=${BUILD_DIR:-/build}
INSTALL_DIR=${INSTALL_DIR:-/usr/local/wasm}
PROJ_SRC_DIR="${BUILD_DIR}/proj_src"
DEPS_SRC_DIR="${BUILD_DIR}/deps_src"
TEMP_BUILD_DIR="${BUILD_DIR}/temp_build"

# Emscripten flags for Pthreads (multithreading support for synchronous emscripten_fetch)
# and some other.
EM_PTHREADS_FLAGS="-pthread -matomics -mbulk-memory -fexceptions"

# --- Utility Functions ---

function log_step {
    echo ""
    echo "--- $1 ---"
    echo ""
}

function configure_cmake {
    # Use emcmake wrapper to correctly configure the toolchain
    emcmake cmake "$@" \
        -G Ninja \
        -D CMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
        -D CMAKE_BUILD_TYPE=Release \
        -D BUILD_SHARED_LIBS=OFF \
        -D CMAKE_C_FLAGS="${EM_PTHREADS_FLAGS}" \
        -D CMAKE_CXX_FLAGS="${EM_PTHREADS_FLAGS}" \
        -D CMAKE_FIND_ROOT_PATH="${INSTALL_DIR}" \
        -D CMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
        -D CMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -D CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY
}

function build_and_install {
    cmake --build . --parallel $(nproc)
    cmake --install .
}

# --- Preparation ---

log_step "1. Setting up Environment and Directories"

if [ "${FORCE_REBUILD}" = "1" ]; then
    echo "!!! FORCE_REBUILD is set. Existing libraries will be ignored and rebuilt. !!!"
    rm -rf "${INSTALL_DIR}"/*
fi

# Clean build directories to ensure a fresh build
rm -rf ${TEMP_BUILD_DIR}
# We keep DEPS_SRC_DIR to cache downloads
mkdir -p ${TEMP_BUILD_DIR}
mkdir -p ${DEPS_SRC_DIR}

if [ -f "${PROJ_SRC_DIR}/CMakeLists.txt" ]; then
    echo "Using content from ${PROJ_SRC_DIR} to build PROJ"
elif [ -n "${PROJ_TAG}" ]; then
    echo "Using PROJ from ${PROJ_REPO} : ${PROJ_TAG}"
else
    echo "Please set PROJ_TAG env variable, or populate ${PROJ_SRC_DIR}"
    exit 1
fi

echo "Installation target: ${INSTALL_DIR}"

# --- 2. Build and Install Zlib (Dependency) ---

log_step "2. Building and Installing Zlib"

if [ "${FORCE_REBUILD}" != "1" ] && [ -f "${INSTALL_DIR}/lib/libz.a" ] && [ -f "${INSTALL_DIR}/include/zlib.h" ]; then
    echo "Zlib library and header already installed. Skipping."
else
    ZLIB_DIR="${DEPS_SRC_DIR}/zlib-1.3.1"
    cd ${DEPS_SRC_DIR}
    if [ ! -f "v1.3.1.zip" ]; then
        wget -nc "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip"
    fi
    unzip -qo v1.3.1.zip
    cd ${ZLIB_DIR}

    # Zlib defines both SHARED and STATIC targets with the same output name (libz.a)
    # on Emscripten, causing Ninja errors.
    # Fixed in https://github.com/madler/zlib/commit/b3907c2cd99908259c71fab6754f88e25d370fed
    # Instead of renaming, we simply remove the SHARED target definitions entirely.
    sed -i '/add_library(zlib SHARED/d' CMakeLists.txt
    sed -i '/target_include_directories(zlib /d' CMakeLists.txt
    sed -i '/set_target_properties(zlib /d' CMakeLists.txt
    sed -i 's/install(TARGETS zlib zlibstatic/install(TARGETS zlibstatic/g' CMakeLists.txt

    rm -rf build_wasm
    mkdir -p build_wasm
    cd build_wasm

    configure_cmake .. -DZLIB_BUILD_EXAMPLES=OFF
    build_and_install

    # Handle case where Zlib installs as libzlibstatic.a instead of libz.a
    if [ -f "${INSTALL_DIR}/lib/libzlibstatic.a" ]; then
        echo "Detected libzlibstatic.a, copying to libz.a..."
        cp "${INSTALL_DIR}/lib/libzlibstatic.a" "${INSTALL_DIR}/lib/libz.a"
    fi
fi

# --- 3. Build and Install LibTIFF (Dependency) ---

log_step "3. Building and Installing LibTIFF"

if [ "${FORCE_REBUILD}" != "1" ] && [ -f "${INSTALL_DIR}/lib/libtiff.a" ] && [ -f "${INSTALL_DIR}/include/tiff.h" ]; then
    echo "LibTIFF library and header already installed. Skipping."
else
    TIFF_VERSION="4.7.0"
    TIFF_DIR="${DEPS_SRC_DIR}/tiff-${TIFF_VERSION}"
    cd ${DEPS_SRC_DIR}

    if [ ! -f "tiff-${TIFF_VERSION}.tar.gz" ]; then
        wget -nc "https://download.osgeo.org/libtiff/tiff-${TIFF_VERSION}.tar.gz"
    fi
    tar -xzf tiff-${TIFF_VERSION}.tar.gz
    cd ${TIFF_DIR}

    rm -rf build_wasm
    mkdir -p build_wasm
    cd build_wasm

    # Configure minimal LibTIFF: No JPEG, No LZMA, No WebP, No ZSTD.
    # Only Zlib support enabled.
    configure_cmake .. \
        -D tiff-tools=OFF \
        -D tiff-tests=OFF \
        -D tiff-contrib=OFF \
        -D tiff-docs=OFF \
        -D jpeg=OFF \
        -D zlib=ON \
        -D lzma=OFF \
        -D zstd=OFF \
        -D webp=OFF \
        -D jbig=OFF \
        -D CMAKE_PREFIX_PATH="${INSTALL_DIR}"

    build_and_install
fi

# --- 4. Build and Install SQLite3 (Dependency) ---

log_step "4. Building and Installing SQLite3"

# Check if both the library AND the header exist before skipping
if [ "${FORCE_REBUILD}" != "1" ] && [ -f "${INSTALL_DIR}/lib/libsqlite3.a" ] && [ -f "${INSTALL_DIR}/include/sqlite3.h" ]; then
    echo "SQLite3 static library and header already found. Skipping build."
else
    SQLITE_VERSION="3440200"
    SQLITE_AMALGAMATION_DIR="${DEPS_SRC_DIR}/sqlite3_amalgamation"
    mkdir -p ${SQLITE_AMALGAMATION_DIR}

    cd ${SQLITE_AMALGAMATION_DIR}
    if [ ! -f "sqlite-amalgamation-${SQLITE_VERSION}.zip" ]; then
        wget "https://sqlite.org/2023/sqlite-amalgamation-${SQLITE_VERSION}.zip"
    fi
    unzip -qo sqlite-amalgamation-${SQLITE_VERSION}.zip
    # Move files to the parent directory for a flat structure
    mv sqlite-amalgamation-${SQLITE_VERSION}/* .
    rmdir sqlite-amalgamation-${SQLITE_VERSION}

    # Step 4a: Compile SQLite3 to an object file (.o)
    emcc sqlite3.c \
        -o ${TEMP_BUILD_DIR}/sqlite3.o \
        -c \
        -O3 \
        -DSQLITE_THREADSAFE=1 \
        -D_REENTRANT \
        ${EM_PTHREADS_FLAGS}

    # Step 4b: Create the static library archive (.a) from the object file
    emmake ar rcs ${INSTALL_DIR}/lib/libsqlite3.a ${TEMP_BUILD_DIR}/sqlite3.o

    # Copy the header to the install directory
    mkdir -p ${INSTALL_DIR}/include
    # Force copy to overwrite any broken symlinks from previous runs
    rm -f ${INSTALL_DIR}/include/sqlite3.h
    cp ${SQLITE_AMALGAMATION_DIR}/sqlite3.h ${INSTALL_DIR}/include/sqlite3.h
fi


# --- 5. Download PROJ Source if needed ---

log_step "5. Downloading PROJ Source"

if [ ! -f "${PROJ_SRC_DIR}/CMakeLists.txt" ]; then
    git clone --depth 1 --branch ${PROJ_TAG} ${PROJ_REPO} ${PROJ_SRC_DIR}
fi

cd ${PROJ_SRC_DIR}

# --- 6. Build and Install PROJ ---
# This step uses the NATIVE sqlite3 binary (from the Docker image)
# to generate proj.db, which is then embedded into libproj.a.

log_step "6. Building and Installing PROJ"

PROJ_BUILD_WASM_DIR="${PROJ_SRC_DIR}/build_wasm"

if [ "${FORCE_REBUILD}" != "1" ] && [ -f "${INSTALL_DIR}/lib/libproj.a" ]; then
    echo "PROJ already built. Skipping."
else
    # If forcing a rebuild, clean the PROJ build directory too to be safe
    if [ "${FORCE_REBUILD}" = "1" ]; then
        echo "Cleaning PROJ build directory due to FORCE_REBUILD..."
        rm -rf ${PROJ_BUILD_WASM_DIR}
    fi

    mkdir -p ${PROJ_BUILD_WASM_DIR}
    cd ${PROJ_BUILD_WASM_DIR}

    # Configure PROJ
    configure_cmake .. \
        -D BUILD_TESTING=OFF \
        -D BUILD_APPS=OFF \
        -D ENABLE_TIFF=ON \
        -D ENABLE_CURL=OFF \
        -D ENABLE_EMSCRIPTEN_FETCH=ON \
        -D EXE_SQLITE3=/usr/bin/sqlite3 \
        -D SQLite3_INCLUDE_DIR="${INSTALL_DIR}/include" \
        -D SQLite3_LIBRARY="${INSTALL_DIR}/lib/libsqlite3.a" \
        -D TIFF_INCLUDE_DIR="${INSTALL_DIR}/include" \
        -D TIFF_LIBRARY_RELEASE="${INSTALL_DIR}/lib/libtiff.a"

    build_and_install
fi

# --- 6.5 Create C Wrappers ---
# Helper functions to extract some info from PROJ in C
log_step "6.5 Creating C Wrapper Functions"

WRAPPER_FILE="${TEMP_BUILD_DIR}/proj_wrappers.c"
WRAPPER_OBJ_FILE="${TEMP_BUILD_DIR}/proj_wrappers.o"

DDD=`date +"%Y-%m-%dT%H:%M:%S%z" -u`
cat << EOF > ${WRAPPER_FILE}
#include "proj.h"
#include "math.h"

const char* get_compilation_date() {
    return "$DDD" ;
}

int get_proj_info_sizeof() {
    return sizeof(PJ_INFO);
}
EOF

# Compile the wrapper into an object file
emcc ${WRAPPER_FILE} \
    -c \
    -o ${WRAPPER_OBJ_FILE} \
    -I ${INSTALL_DIR}/include \
    ${EM_PTHREADS_FLAGS}

# --- 7. Final WASM Module Generation ---
# Link all the static libraries and the wrapper object file
# into the final JS/WASM module and expose functions.

log_step "7. Generating Final WASM Module (projModule.js + .wasm)"

FINAL_LIBS="${INSTALL_DIR}/lib/libproj.a \
            ${INSTALL_DIR}/lib/libsqlite3.a \
            ${INSTALL_DIR}/lib/libtiff.a \
            ${INSTALL_DIR}/lib/libz.a \
            ${WRAPPER_OBJ_FILE}"

emcc -v ${FINAL_LIBS} \
    -o ${INSTALL_DIR}/projModule.js \
    -O3 \
    -s STACK_OVERFLOW_CHECK=1 \
    -s STACK_SIZE=5MB \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    -s FETCH=1 \
    -s USE_PTHREADS=1 \
    -s FETCH_SUPPORT_INDEXEDDB=0 \
    -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="'ProjModuleFactory'" \
    -s FORCE_FILESYSTEM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s EXPORTED_RUNTIME_METHODS="[ccall, cwrap, FS, HEAPF64, stringToNewUTF8, UTF8ToString, getValue]" \
    -s EXPORTED_FUNCTIONS="[
     _get_proj_info_sizeof,
     _get_compilation_date,
     _proj_info,
     _proj_context_errno_string,
     _proj_context_create,
     _proj_context_set_enable_network,
     _proj_create,
     _proj_create_from_database,
     _proj_create_crs_to_crs,
     _proj_create_crs_to_crs_from_pj,
     _proj_trans,
     _proj_trans_array,
     _proj_context_destroy,
     _proj_destroy,
     _malloc, _free
     ]" \
    ${EM_PTHREADS_FLAGS}

log_step "BUILD SUCCESSFUL!"

echo "ls -l ${INSTALL_DIR}"
ls -l ${INSTALL_DIR}

echo "Artifacts are installed in ${INSTALL_DIR}"
