cmake_minimum_required(VERSION 3.5)

project(vsodium LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(vsodium.cmake)

set(VLIBS_DIR ../vlibs2)
include(${VLIBS_DIR}/cmake/vlog.cmake)

add_executable(${PROJECT_NAME} main.cpp ${VSODIUM_SOURCES} ${V_HEADERS} ${V_SOURCES})

target_link_libraries(${PROJECT_NAME} ${VSODIUM_LIBRARIES})
