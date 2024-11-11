cmake_minimum_required(VERSION 3.10)
project(Lua_devel C)

set(lua_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/lapi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lauxlib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lbaselib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lcode.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lcorolib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lctype.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ldblib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ldebug.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ldo.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ldump.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lfunc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lgc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/linit.c
  ${CMAKE_CURRENT_SOURCE_DIR}/liolib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/llex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lmathlib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lmem.c
  ${CMAKE_CURRENT_SOURCE_DIR}/loadlib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lobject.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lopcodes.c
  ${CMAKE_CURRENT_SOURCE_DIR}/loslib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lparser.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lstate.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lstring.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lstrlib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ltable.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ltablib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ltests.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ltm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lua.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lundump.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lutf8lib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lvm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lzio.c)

add_library(lua
  STATIC
  ${lua_sources})

if(UNIX)
  target_compile_definitions(lua
    PUBLIC
    LUA_USE_C89
    LUA_USE_LINUX)
endif()

if(WIN32)
  target_compile_definitions(lua
    PUBLIC
    LUA_BUILD_AS_DLL)
  target_link_libraries(lua
    PUBLIC
    ucrt
    legacy_stdio_definitions
    kernel32
    user32
    gdi32
    winspool
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    uuid
    odbc32
    odbccp32)
endif()

set_target_properties(lua PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_CURRENT_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_CURRENT_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_CURRENT_BINARY_DIR})
