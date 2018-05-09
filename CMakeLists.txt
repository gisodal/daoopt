# CMake file for DAOOPT solver
# by Lars Otten, 2012

cmake_minimum_required(VERSION 2.6)
project(DAOOPT)

# To select the two master modes
option(DAOOPT_WORKER "Force worker binary" OFF)
option(DAOOPT_MASTER_STATIC "Force static master binary" OFF)
option(DAOOPT_MASTER_DYNAMIC "Force dynamic master binary" OFF)

# To enable static linking of the final daoopt binary
option(DAOOPT_LINK_STATIC "Link binary statically" OFF)

# General Compiler flags
add_definitions(-Wall)

if(WIN32)
  add_definitions(-DWINDOWS)
else()
  add_definitions(-DLINUX)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -DDEBUG -O0 -g3")
set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -DDEBUG -O0 -g3")

set(CMAKE_CXX_FLAGS_RELEASE "-w -O3")
set(CMAKE_C_FLAGS_RELEASE "-w -O3")

if(DAOOPT_LINK_STATIC)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

# Optional forced version: static over dynamic master, worker over both
if(DAOOPT_WORKER)
  add_definitions(-DNOTHREADS)
else()
  if(DAOOPT_MASTER_STATIC)
    add_definitions(-DPARALLEL_STATIC)
  endif()
  if(DAOOPT_MASTER_DYNAMIC AND NOT DAOOPT_MASTER_STATIC)
    add_definitions(-DPARALLEL_DYNAMIC)
    find_package(Threads)  # includes pthread
    find_package( Boost REQUIRED COMPONENTS system )
  endif()
endif()

find_package( Boost REQUIRED COMPONENTS program_options thread )
include_directories( ${Boost_INCLUDE_DIRS} )

# Included folders for header files
include_directories(
  ${DAOOPT_SOURCE_DIR}/include
  ${DAOOPT_SOURCE_DIR}/lib
)

# Libraries in the lib folder via recursive CMakeLists.txt
add_subdirectory(lib)
set(LIBS Gzstream Sls4mpe Mex Cvo)

# Main daoopt source files
file(GLOB FILES "${CMAKE_CURRENT_LIST_DIR}/source/*.cpp")

# Main executable and library dependencies
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif()
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

set(PROJECT "daoopt")
add_executable(${PROJECT}-bin daoopt.cpp ${FILES})
set_target_properties(${PROJECT}-bin PROPERTIES OUTPUT_NAME ${PROJECT})
target_link_libraries(${PROJECT}-bin ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

add_library(${PROJECT} STATIC ${FILES})
target_link_libraries(${PROJECT} ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

file(GLOB HEADERS "${INCLUDE_DIR}/*.h")
set_target_properties(${PROJECT} PROPERTIES PUBLIC_HEADER "${HEADERS}")
install(TARGETS ${PROJECT}
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    PUBLIC_HEADER DESTINATION "include/daoopt"
)

