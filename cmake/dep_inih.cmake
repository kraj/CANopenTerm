cmake_minimum_required(VERSION 3.10)
project(inih_devel C)

set(inih_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/ini.c)

add_library(inih
  STATIC
  ${inih_sources})

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  target_link_libraries(inih
    PUBLIC
    ucrt
    legacy_stdio_definitions)
endif()

set_target_properties(inih PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_CURRENT_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_CURRENT_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_CURRENT_BINARY_DIR})
