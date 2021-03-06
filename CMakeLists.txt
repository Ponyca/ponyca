cmake_minimum_required(VERSION 2.8)
project(ponyca)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(PrecompiledHeader)

# Enable C++11 on g++/clang++
if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_definitions(-std=c++0x)
    endif()
endif(UNIX)

# Common
file(GLOB_RECURSE common_sources common/*.cpp)
include_directories(common/ ${CMAKE_SOURCE_DIR})
add_library(common SHARED ${common_sources} build/opcodes.cpp)

if(UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(common PROPERTIES PCH_DEFINITIONS -fPIC)
endif()
add_precompiled_header(common common/pch.h)


# Client
file(GLOB_RECURSE client_sources client/*.cpp)
include_directories(client/ ${CMAKE_SOURCE_DIR})
add_executable(client ${client_sources})

# Server
file(GLOB_RECURSE server_sources server/*.cpp)
include_directories(server/ ${CMAKE_SOURCE_DIR})
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

# ASIO
find_package(ASIO REQUIRED)
include_directories(${ASIO_INCLUDE_DIRS})
link_directories(${ASIO_LINK_DIRS})
add_definitions(${ASIO_DEFINITIONS})
target_link_libraries(common ${ASIO_LIBRARIES})

# Threads
set(CMAKE_THREAD_PREFER_PTHREAD)
find_package(Threads REQUIRED)
target_link_libraries(common ${CMAKE_THREAD_LIBS_INIT})

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
include_directories(${CMAKE_SOURCE_DIR}/libs/utfcpp)
include_directories(${CMAKE_SOURCE_DIR}/libs/utfcpp/utf8)

# Dynlibs
set_target_properties(common PROPERTIES LINK_FLAGS "-ldl")


# Opcodes generator
add_custom_target(opcodes COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/utils/opcodes_generator.py" "${CMAKE_CURRENT_SOURCE_DIR}/common/opcodes.yml" "${CMAKE_SOURCE_DIR}/build/opcodes.h" "${CMAKE_SOURCE_DIR}/build/opcodes.cpp"
    COMMENT "Generating opcodes..."
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(common opcodes)

# Tests
find_package(CxxTest REQUIRED)
include_directories(${CXXTEST_INCLUDE_DIRS})
link_directories(${CXXTEST_LINK_DIRS})
add_definitions(${CXXTEST_DEFINITIONS})
enable_testing()
cxxtest_add_test(testsrunner testsrunner.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_serialization.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_config.cpp)
target_link_libraries(testsrunner common)
