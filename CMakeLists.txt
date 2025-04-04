cmake_minimum_required(VERSION 3.10)

# Name your project
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

# Add include directory
file(GLOB_RECURSE SUBDIRS RELATIVE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/*)

set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)  # Include main src folder

foreach(dir ${SUBDIRS})
    get_filename_component(dir ${dir} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/${dir})
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})



set(SOURCES
	src/Main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug")