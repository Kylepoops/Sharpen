cmake_minimum_required(VERSION 3.14.0)

add_executable(Console "${PROJECT_SOURCE_DIR}/examples/CommandLines/Console.cpp")

target_link_libraries(Console sharpen)