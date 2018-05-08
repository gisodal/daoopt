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

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -O0 -g3")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0 -g3")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_RELEASE "-O3")

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
include_directories(${INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

message(STATUS "TOP: ${CMAKE_INSTALL_PREFIX}")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

add_executable(daoopt daoopt.cpp ${FILES})
target_link_libraries(daoopt bn-to-cnf ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

project(pseudotree)
add_library(pseudotree STATIC ${FILES})
target_link_libraries(pseudotree bn-to-cnf)

set_target_properties(pseudotree PROPERTIES PUBLIC_HEADER "./include/Pseudotree.h")
install(TARGETS pseudotree
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    PUBLIC_HEADER DESTINATION "include/pseudotree"
)



