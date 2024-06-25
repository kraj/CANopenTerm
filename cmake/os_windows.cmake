# SDL2
set(SDL2_VERSION "2.30.4")

set(SDL2_DEVEL_PKG SDL2-devel-${SDL2_VERSION}-VC.zip)
set(SDL2_PLATFORM  "x64")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(SDL2_PLATFORM "x86")
endif()

set(SDL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2-${SDL2_VERSION}_${SDL2_PLATFORM})

ExternalProject_Add(SDL2_devel
  URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/${SDL2_DEVEL_PKG}
  URL_HASH SHA1=cbc125e68b0172f48dc5e15aad0c3a470c2a646a
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${SDL2_PATH}/
  BUILD_BYPRODUCTS ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2.lib
  BUILD_BYPRODUCTS ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2main.lib

  BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
    ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2.dll ${CMAKE_CURRENT_SOURCE_DIR}/export

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL2_devel.cmake" ${SDL2_PATH}/CMakeLists.txt)

set(SDL2_INCLUDE_DIR ${SDL2_PATH}/include)
set(SDL2_LIBRARY     ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2.lib)
set(SDL2MAIN_LIBRARY ${SDL2_PATH}/lib/${SDL2_PLATFORM}/SDL2main.lib)

# PCAN-Basic API
set(PCAN_VERSION_WINDOWS "4.9.0.942")

set(PCAN_PLATFORM  "x64")
set(PCAN_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/PCAN-Basic_API_Windows")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(PCAN_PLATFORM "Win32")
endif()

set(PCAN_DEVEL_PKG  PCAN-Basic_Windows-${PCAN_VERSION_WINDOWS}.zip)
set(PCAN_DEVEL_URL  https://canopenterm.de/mirror)
set(PCAN_DEVEL_HASH 5aa4459340986d921a63f15cc643733ab7d9c011)

ExternalProject_Add(PCAN_devel
  URL ${PCAN_DEVEL_URL}/${PCAN_DEVEL_PKG}
  URL_HASH SHA1=${PCAN_DEVEL_HASH}
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${PCAN_PATH}/
  BUILD_BYPRODUCTS ${PCAN_PATH}/${PCAN_PLATFORM}/VC_LIB/PCANBasic.lib

  BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
  ${PCAN_PATH}/${PCAN_PLATFORM}/PCANBasic.dll ${CMAKE_CURRENT_SOURCE_DIR}/export

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PCAN_devel.cmake" ${PCAN_PATH}/CMakeLists.txt)

set(PCAN_INCLUDE_DIR ${PCAN_PATH}/Include)
set(PCAN_LIBRARY     ${PCAN_PATH}/${PCAN_PLATFORM}/VC_LIB/PCANBasic.lib)

# dirent
set(DIRENT_VERSION   "1.24")
set(DIRENT_PATH      ${CMAKE_CURRENT_SOURCE_DIR}/deps/dirent-${DIRENT_VERSION})
set(DIRENT_DEVEL_PKG ${DIRENT_VERSION}.zip)

ExternalProject_Add(dirent_devel
  URL https://github.com/tronkko/dirent/archive/refs/tags/${DIRENT_DEVEL_PKG}
  URL_HASH SHA1=70b02369071572dd1b080057a6b9170dec04868d
  DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
  DOWNLOAD_NO_PROGRESS true
  TLS_VERIFY true
  SOURCE_DIR ${DIRENT_PATH}/

  BUILD_COMMAND   ${CMAKE_COMMAND} -E echo "Skipping build step."
  INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step."

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dirent_devel.cmake" ${DIRENT_PATH}/CMakeLists.txt)

set(DIRENT_INCLUDE_DIR ${DIRENT_PATH}/include)

set(os_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/os/can_windows.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/os/os_windows.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/os/scripts_windows.c)

set(PALTFORM_DEPS PCAN_devel)
set(PLATFORM_LIBS ${PCAN_LIBRARY})
