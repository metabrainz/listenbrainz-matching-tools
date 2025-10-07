cmake_minimum_required(VERSION 3.15)

# Find Google Test
find_package(GTest)

if(NOT GTest_FOUND)
    # If GTest is not found, try to use FetchContent to download it
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Create aliases for consistent usage
    add_library(GTest::gtest ALIAS gtest)
    add_library(GTest::gtest_main ALIAS gtest_main)
endif()

# Test sources
set(TEST_SOURCES
    test_cleaner.cpp
    test_guff.cpp
)

# Create test executable
add_executable(lb_matching_tests ${TEST_SOURCES})

# Link against the header-only library and required dependencies
target_link_libraries(lb_matching_tests
    PRIVATE
        lb_matching_tools
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(lb_matching_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(lb_matching_tests)