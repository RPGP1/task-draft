cmake_minimum_required(VERSION 3.5)

project(task-draft CXX C)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++1z" CAN_USE_CXX1Z)
if (NOT CAN_USE_CXX1Z)
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER} doesn't support C++1z.\n")
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wswitch-default -Wdisabled-optimization -Wformat -Winit-self -Woverloaded-virtual -Wfloat-equal -Wno-old-style-cast -Wno-pragmas")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")


file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(main ${SOURCE_FILES} test_main.cpp)
target_include_directories(main PUBLIC include)
