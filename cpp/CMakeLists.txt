cmake_minimum_required(VERSION 3.15)

project(lb-matching-tools
    VERSION 1.0.0
    DESCRIPTION "ListenBrainz tools for matching metadata - Header-only C++ library"
    LANGUAGES CXX
)

# Set C++17 as the standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compile options for different compilers
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Options
option(LB_MATCHING_BUILD_TESTS "Build tests" ON)
option(LB_MATCHING_BUILD_EXAMPLES "Build examples" ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCRE2 REQUIRED libpcre2-8)

if(NOT PCRE2_FOUND)
    # Fallback: try to find PCRE2 manually
    find_library(PCRE2_LIBRARIES NAMES pcre2-8 pcre2)
    find_path(PCRE2_INCLUDE_DIRS NAMES pcre2.h)
    
    if(NOT PCRE2_LIBRARIES OR NOT PCRE2_INCLUDE_DIRS)
        message(FATAL_ERROR "PCRE2 library not found. Please install libpcre2-dev or equivalent.")
    endif()
    
    set(PCRE2_CFLAGS "")
    set(PCRE2_LDFLAGS "")
endif()

# Create header-only library target
add_library(lb_matching_tools INTERFACE)

# Include directories for header-only library
target_include_directories(lb_matching_tools
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PCRE2_INCLUDE_DIRS}
)

# Link libraries for header-only library
target_link_libraries(lb_matching_tools
    INTERFACE
        ${PCRE2_LIBRARIES}
)

# Add PCRE2 compile definitions
target_compile_definitions(lb_matching_tools INTERFACE ${PCRE2_CFLAGS_OTHER})
if(PCRE2_LDFLAGS)
    target_link_options(lb_matching_tools INTERFACE ${PCRE2_LDFLAGS})
endif()

# Examples
if(LB_MATCHING_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(LB_MATCHING_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
