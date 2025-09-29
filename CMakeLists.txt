# Specify the minimum version of CMake required to build this project.
cmake_minimum_required(VERSION 3.10)

# Define the project name. This will be the name of the solution in Visual Studio
# or the makefile in a Unix-like environment.
project(FPGA_Router)

# Set the C++ standard to C++17. The parser code uses features from this standard.
# CMAKE_CXX_STANDARD is the modern way to specify the standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the directory where header files are located.
# This allows the compiler to find #include "DataTypes.hpp" etc.
include_directories(include)

# Define the name of the final executable file.
set(EXECUTABLE_NAME FRouter)

# Find all source files in the src/ directory and add them to the SOURCES variable.
# This is a convenient way to manage source files without listing each one manually.
file(GLOB SOURCES "src/*.cpp")

# Create the executable target by compiling and linking the specified source files.
add_executable(${EXECUTABLE_NAME} ${SOURCES})


if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # 对于GCC和Clang
    target_link_libraries(FRouter PRIVATE -lstdc++fs)
endif()

# Set the output directory for the compiled executable.
# ${CMAKE_BINARY_DIR} corresponds to the 'build' directory where you run cmake.
# This ensures that 'FRouter' will be created inside 'build/'.
set_target_properties(${EXECUTABLE_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Optional: Print a message after configuration is done.
message(STATUS "Configuration complete. Executable will be built as: ${CMAKE_BINARY_DIR}/${EXECUTABLE_NAME}")
