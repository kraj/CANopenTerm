cmake_minimum_required(VERSION 3.10)
project(inih_devel C)

set(inih_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/ini.c)

add_library(inih
  STATIC
  ${inih_sources})

set_target_properties(inih PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
