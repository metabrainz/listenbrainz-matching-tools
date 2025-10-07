cmake_minimum_required(VERSION 3.15)

# Simple example showing how to use lb_matching_tools
add_executable(simple_example simple_example.cpp)

target_link_libraries(simple_example
    PRIVATE
        lb_matching_tools
)

target_include_directories(simple_example
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)