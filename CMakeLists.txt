cmake_minimum_required(VERSION 3.28)
project(myProgram)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the C and C++ compilers explicitly
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Remove compiler identification files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

# Exclude problematic
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}/CMakeFiles/.*CMakeCXXCompilerId.cpp")
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}/CMakeFiles/.*CMakeCCompilerId.c")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Add the -g flag for debug information
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Include directories for the target
target_include_directories(${PROJECT_NAME} PUBLIC "C:/msys64/ucrt64/include")

# Link other required libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw3 gdi32 glu32 opengl32)
