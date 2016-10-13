cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++11)
project( ss )

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})

link_directories(${X11_LIBRARIES})

add_executable(ss
    CImg.h
    SS.h
    SS.cpp
    main.cpp
)

target_link_libraries(ss ${X11_LIBRARIES})
target_link_libraries(ss ${CMAKE_THREAD_LIBS_INIT})
