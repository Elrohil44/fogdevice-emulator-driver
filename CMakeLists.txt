cmake_minimum_required(VERSION 3.17)
project(fogdevice_emulator_driver C)


set(CMAKE_C_STANDARD 99)
set (FOG_DEVICE_TARGET fogdevice_emulator_driver)
include("${PROJECT_SOURCE_DIR}/src/paho.cmake")

include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB SOURCES
     LIST_DIRECTORIES false
     "${PROJECT_SOURCE_DIR}/src/*.c"
     "${PROJECT_SOURCE_DIR}/src/spp/*.c")

add_executable(${FOG_DEVICE_TARGET} ${SOURCES})
target_link_libraries(${FOG_DEVICE_TARGET} ${PAHO_TARGETS})
