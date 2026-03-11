cmake_minimum_required(VERSION 3.5)

project(vsodium LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(vsodium.cmake)

set(VLIBS_DIR ../vlibs2)
include(${VLIBS_DIR}/cmake/vlog.cmake)
include(${VLIBS_DIR}/cmake/vcat.cmake)
include(${VLIBS_DIR}/cmake/vbyte_buffer.cmake)
#include(${VLIBS_DIR}/cmake/vtime_meter.cmake)

add_executable(${PROJECT_NAME} main.cpp ${VSODIUM_SOURCES} ${V_HEADERS} ${V_SOURCES}
    vsodium_emodji.h vsodium_emodji.cpp)

target_link_libraries(${PROJECT_NAME} ${VSODIUM_LIBRARIES})
