include_guard()

cmake_minimum_required(VERSION 3.5)

set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_init.h)
set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_init.cpp)

set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_call_checker.h)
set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_call_checker.cpp)

set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_string.h)
set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_string.cpp)

set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_curve25519.h)
set(VSODIUM_SOURCES ${VSODIUM_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/vsodium_curve25519.cpp)

set(VSODIUM_LIBRARIES ${VSODIUM_LIBRARIES} sodium)
#set(V_LIBRARIES ${V_LIBRARIES} ${VSODIUM_LIBRARIES})
