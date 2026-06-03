cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_l7_task1)

# Add main source file
target_sources(app PRIVATE src/main.cpp)

# Add driver subdirectory
add_subdirectory(../drivers/our_driver ${CMAKE_CURRENT_BINARY_DIR}/our_driver)
