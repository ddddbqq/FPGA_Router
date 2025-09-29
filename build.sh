#!/bin/bash

# This script automates the CMake build process.
# It handles configuration, compilation, and optional execution or cleaning.

# Exit immediately if a command exits with a non-zero status.
# This prevents errors from going unnoticed.
set -e

# --- Configuration ---
# The name of the final executable file, as defined in CMakeLists.txt.
EXECUTABLE_NAME="FRouter"
# The directory where build files will be generated.
BUILD_DIR="build"

# --- Helper Functions for Colored Output ---
# Makes the script output easier to read.
print_info() {
    echo -e "\033[1;34m[INFO] $1\033[0m"
}

print_success() {
    echo -e "\033[1;32m[SUCCESS] $1\033[0m"
}

print_error() {
    echo -e "\033[1;31m[ERROR] $1\033[0m" >&2
}

# Function to clean the build directory.
clean_project() {
    if [ -d "$BUILD_DIR" ]; then
        print_info "Cleaning the project by removing the '$BUILD_DIR' directory..."
        rm -rf "$BUILD_DIR"
        print_success "Project cleaned."
    else
        print_info "Build directory not found. Nothing to clean."
    fi
}

# --- Script Main Logic ---

# Function to perform the build.
build_project() {
    # Always clean before building
    clean_project
    
    print_info "Starting build process..."

    # Create the build directory if it doesn't exist.
    if [ ! -d "$BUILD_DIR" ]; then
        print_info "Build directory not found. Creating '$BUILD_DIR'..."
        mkdir "$BUILD_DIR"
    fi

    # Navigate into the build directory.
    cd "$BUILD_DIR"

    # Run CMake to configure the project. The '..' points to the parent directory
    # where CMakeLists.txt is located.
    print_info "Configuring project with CMake..."
    cmake ..

    # Compile the project using make.
    # The -j flag allows for parallel compilation, which can speed up the process.
    # We try to detect the number of available CPU cores.
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        CORES=$(nproc)
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=1 # Default to 1 core if OS is not detected.
    fi
    print_info "Compiling project with make on $CORES cores..."
    make -j"$CORES"

    print_success "Build completed successfully. Executable is at: $BUILD_DIR/$EXECUTABLE_NAME"
    
    # Navigate back to the project root directory.
    cd ..
}

# --- Argument Parsing ---
# The script's behavior changes based on the first argument provided.
case "$1" in
    "")
        # No argument: just build the project.
        build_project
        ;;
    "run")
        # 'run' argument: build and then execute.
        build_project
        print_info "Executing the program..."
        echo "-------------------------------------"
        ./"$BUILD_DIR"/"$EXECUTABLE_NAME"
        echo "-------------------------------------"
        print_success "Program finished."
        ;;
    "clean")
        # 'clean' argument: remove the build directory.
        clean_project
        ;;
    *)
        # Invalid argument: show usage help.
        print_error "Invalid argument: $1"
        echo "Usage: $0 [run|clean]"
        echo "  (no argument) : Builds the project."
        echo "  run           : Builds the project and runs the executable."
        echo "  clean         : Removes the build directory."
        exit 1
        ;;
esac