cmake_minimum_required(VERSION 2.8)
project(ponyca)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Common
file(GLOB_RECURSE common_sources common/*.cpp)
include_directories(common/)
add_library(common SHARED ${common_sources})

# Client
file(GLOB_RECURSE client_sources client/*.cpp)
include_directories(client/ common/)
add_executable(client ${client_sources})

# Server
file(GLOB_RECURSE server_sources server/*.cpp)
include_directories(server/ common/)
add_executable(server ${server_sources})

#add_subdirectory(plugins)

# Common link
target_link_libraries(client common)
target_link_libraries(server common)

# Zlib
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
link_directories(${ZLIB_LINK_DIRS})
add_definitions(${ZLIB_DEFINITIONS})
target_link_libraries(common ${ZLIB_LIBRARIES})

# MsgPack
find_package(MsgPack REQUIRED)
include_directories(${MSGPACK_INCLUDE_DIRS})
link_directories(${MSGPACK_LINK_DIRS})
add_definitions(${MSGPACK_DEFINITIONS})
target_link_libraries(common ${MSGPACK_LIBRARIES})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LINK_DIRS})
add_definitions(${OPENGL_DEFINITIONS})
target_link_libraries(client ${OPENGL_LIBRARIES})

# GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LINK_DIRS})
add_definitions(${GLUT_DEFINITIONS})
target_link_libraries(client ${GLUT_LIBRARIES})

# FreeType
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LINK_DIRS})
add_definitions(${FREETYPE_DEFINITIONS})
target_link_libraries(client ${FREETYPE_LIBRARIES})

# OpenAL
find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIRS})
link_directories(${OPENAL_LINK_DIRS})
add_definitions(${OPENAL_DEFINITIONS})
target_link_libraries(client ${OPENAL_LIBRARIES})

# UTF8-CPP
find_package(UTF8CPP REQUIRED)
include_directories(${UTF8CPP_INCLUDE_DIRS})
link_directories(${UTF8CPP_LINK_DIRS})
add_definitions(${UTF8CPP_DEFINITIONS})
target_link_libraries(common ${UTF8CPP_LIBRARIES})

# Dynlibs
set_target_properties(common PROPERTIES LINK_FLAGS "-ldl")

# Enable C++11 on g++/clang++
if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_definitions(-std=c++11)
    endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
endif(UNIX)
