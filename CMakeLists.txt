cmake_minimum_required(VERSION 3.23)
project(signal_labs_function_plotting)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
#find_package(SDL2 REQUIRED)
add_executable(signal_labs_function_plotting TurbinaSzczescia.cpp)

include_directories( ${OpenGL_INCLUDE_DIRS} )
target_link_libraries(signal_labs_function_plotting ${OpenGL_LIBS} )
#include_directories( ${SDL2_INCLUDE_DIRS} )
#target_link_libraries( signal_labs_function_plotting ${SDL2_LIBS} )


