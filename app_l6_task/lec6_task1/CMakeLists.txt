cmake_minimum_required(VERSION 3.20.0)

# list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_l6_task)

target_sources(app PRIVATE src/main.cpp)
add_subdirectory(../drivers/our_driver ${CMAKE_CURRENT_BINARY_DIR}/our_driver)
 