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
  set_target_properties(lua PROPERTIES OUTPUT_NAME "lua" PREFIX "" SUFFIX ".lib")
endif(WIN32)
