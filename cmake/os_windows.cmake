# inih
set(INIH_VERSION     "58")
set(INIH_DEVEL_PKG   r${INIH_VERSION}.zip)
set(INIH_PATH        ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}/inih-${INIH_VERSION})
set(INIH_INCLUDE_DIR ${INIH_PATH})
set(INIH_LIBRARY     ${INIH_PATH}_build/inih.lib)

ExternalProject_Add(inih_devel
  URL https://github.com/benhoyt/inih/archive/refs/tags/${INIH_DEVEL_PKG}
  URL_HASH SHA1=4ab39673da3a84ccf9828428616acced69f0528e
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${INIH_PATH}/
  BINARY_DIR ${INIH_PATH}_build/
  BUILD_BYPRODUCTS ${INIH_LIBRARY}

  INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step."

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_inih.cmake" ${INIH_PATH}/CMakeLists.txt)

# Lua
set(LUA_VERSION     "5.4.6")
set(LUA_DEVEL_PKG   v${LUA_VERSION}.zip)
set(LUA_PATH        ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}/lua-${LUA_VERSION}_${PLATFORM})
set(LUA_INCLUDE_DIR ${LUA_PATH})
set(LUA_LIBRARY     ${LUA_PATH}_build/lua.lib)

ExternalProject_Add(Lua_devel
  URL https://github.com/lua/lua/archive/refs/tags/${LUA_DEVEL_PKG}
  URL_HASH SHA1=96abb80f46e2c6548b47632384205bddfaeb6c37
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${LUA_PATH}/
  BINARY_DIR ${LUA_PATH}_build/
  BUILD_BYPRODUCTS ${LUA_LIBRARY}

  INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step."

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_lua.cmake" ${LUA_PATH}/CMakeLists.txt)

# SDL2
set(SDL2_VERSION "2.30.5")

set(SDL2_DEVEL_PKG SDL2-devel-${SDL2_VERSION}-VC.zip)
set(SDL2_PLATFORM  "x64")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(SDL2_PLATFORM "x86")
endif()

set(SDL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}/SDL2-${SDL2_VERSION}_${SDL2_PLATFORM})

ExternalProject_Add(SDL2_devel
  URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/${SDL2_DEVEL_PKG}
  URL_HASH SHA1=035842a7061f3ab4f9d469bab9a3874587a07c74
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${SDL2_PATH}/
  BUILD_BYPRODUCTS ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2.lib
  BUILD_BYPRODUCTS ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2main.lib

  BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
    ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2.dll ${CMAKE_CURRENT_SOURCE_DIR}/export

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_sdl2.cmake" ${SDL2_PATH}/CMakeLists.txt)

set(SDL2_INCLUDE_DIR ${SDL2_PATH}/include)
set(SDL2_LIBRARY     ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2.lib)
set(SDL2MAIN_LIBRARY ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2main.lib)

# PCAN-Basic API
set(PCAN_VERSION_WINDOWS "4.9.0.942")

set(PCAN_PLATFORM  "x64")
set(PCAN_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}/PCAN-Basic_API_Windows")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(PCAN_PLATFORM "Win32")
endif()

set(PCAN_DEVEL_PKG  PCAN-Basic_Windows-${PCAN_VERSION_WINDOWS}.zip)
set(PCAN_DEVEL_URL  https://canopenterm.de/mirror)
set(PCAN_DEVEL_HASH 5aa4459340986d921a63f15cc643733ab7d9c011)

ExternalProject_Add(PCAN_devel
  URL ${PCAN_DEVEL_URL}/${PCAN_DEVEL_PKG}
  URL_HASH SHA1=${PCAN_DEVEL_HASH}
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${PCAN_PATH}/
  BUILD_BYPRODUCTS ${PCAN_PATH}/${PCAN_PLATFORM}/VC_LIB/PCANBasic.lib

  BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
  ${PCAN_PATH}/${PCAN_PLATFORM}/PCANBasic.dll ${CMAKE_CURRENT_SOURCE_DIR}/export

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_pcan.cmake" ${PCAN_PATH}/CMakeLists.txt)

set(PCAN_INCLUDE_DIR ${PCAN_PATH}/Include)
set(PCAN_LIBRARY     ${PCAN_PATH}/${PCAN_PLATFORM}/VC_LIB/PCANBasic.lib)

# dirent
set(DIRENT_VERSION   "1.24")
set(DIRENT_PATH      ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}/dirent-${DIRENT_VERSION})
set(DIRENT_DEVEL_PKG ${DIRENT_VERSION}.zip)

ExternalProject_Add(dirent_devel
  URL https://github.com/tronkko/dirent/archive/refs/tags/${DIRENT_DEVEL_PKG}
  URL_HASH SHA1=70b02369071572dd1b080057a6b9170dec04868d
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps_${PLATFORM}
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${DIRENT_PATH}/

  BUILD_COMMAND   ${CMAKE_COMMAND} -E echo "Skipping build step."
  INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step."

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_dirent.cmake" ${DIRENT_PATH}/CMakeLists.txt)

set(DIRENT_INCLUDE_DIR ${DIRENT_PATH}/include)

add_dependencies(${PROJECT_NAME} PCAN_devel)

set(PLATFORM_LIBS
  ${PCAN_LIBRARY}
  ${SDL2_LIBRARY}
  ${SDL2MAIN_LIBRARY}
  ${INIH_LIBRARY}
  ${LUA_LIBRARY})

set(PLATFORM_CORE_DEPS
  inih_devel
  Lua_devel
  SDL2_devel)

include_directories(
  SYSTEM ${SDL2_INCLUDE_DIR}
  SYSTEM ${PCAN_INCLUDE_DIR}
  SYSTEM ${PCAN_INCLUDE_DIR}/../src/pcan/driver
  SYSTEM ${PCAN_INCLUDE_DIR}/../src/pcan/lib
  SYSTEM ${LUA_INCLUDE_DIR}
  SYSTEM ${INIH_INCLUDE_DIR}
  SYSTEM ${DIRENT_INCLUDE_DIR})

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
