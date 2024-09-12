cmake_minimum_required(VERSION 3.10)

project(flower LANGUAGES C)

# Automatically determine platform name
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(PLATFORM_NAME "linux")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_NAME "macos")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLATFORM_NAME "windows")
else()
    set(PLATFORM_NAME "unknown")
endif()

# Platform-specific compiler configurations
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_FLAGS "-O2 -Wall -Wno-pedantic -pthread")
    set(LINK_FLAGS "")
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(COMPILE_FLAGS "-O2 -Wall -Wno-pedantic -pthread")
    set(LINK_FLAGS "")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(COMPILE_FLAGS "/O2 /Wall")
    set(LINK_FLAGS "")
endif()

# Set source files from the src directory
set(SOURCES
    src/main.c
    src/mniplist.c
    src/printrpt.c
    src/memfunc.c
    src/updtlist.c
)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Define output directory for executables (bin above the build directory)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add executable with platform name in the bin directory
add_executable(flower-${PLATFORM_NAME} ${SOURCES})

# Set compile flags
target_compile_options(flower-${PLATFORM_NAME} PRIVATE ${COMPILE_FLAGS})

# Set linker flags
target_link_libraries(flower-${PLATFORM_NAME} PRIVATE ${LINK_FLAGS})
