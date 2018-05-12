cmake_minimum_required(VERSION 3.2)
project(di)

# register modules
include(CTest)
include(ExternalProject)

# set C++ standard and compiler settings
set(CMAKE_CXX_STANDARD 14)
add_definitions(-Wall -Werror -Wfatal-errors)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread")
    set(RT_LIBRARY "rt")
endif()

# find boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.61.0 COMPONENTS system program_options date_time filesystem regex log REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


# add src to includes
include_directories(src)

#Setup CMake to run tests
enable_testing()

# include subdirectories
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)

