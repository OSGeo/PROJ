# Use the official Emscripten SDK image.
FROM emscripten/emsdk:4.0.5

# Set non-interactive frontend for package installations
ENV DEBIAN_FRONTEND="noninteractive"

# --- Install System Dependencies ---
RUN apt-get update && \
    apt-get install -y \
    sqlite3 \
    ninja-build && \
    # Clean up apt cache to reduce image size
    rm -rf /var/lib/apt/lists/*

# --- Setup Build Directories ---
# Define standard locations for building and installing
ENV BUILD_DIR="/build"
ENV INSTALL_DIR="/usr/local/wasm"

# Create the directories
RUN mkdir -p ${BUILD_DIR}/proj_src \
             ${BUILD_DIR}/deps_src \
             ${INSTALL_DIR}

# Copy the build script into the container
COPY build_wasm.sh /scripts/build_wasm.sh
RUN chmod +x /scripts/build_wasm.sh

# Set the main command to execute the build script
WORKDIR /build
CMD ["/scripts/build_wasm.sh"]
