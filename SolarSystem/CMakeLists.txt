cmake_minimum_required(VERSION 2.6)
SET(EXE_NAME SolarSystem)

# Specify these before creating target
link_directories($ENV{GLUT_LIB} $ENV{OPENCV_LIB}) 

include_directories($ENV{GLUT_INCLUDE} $ENV{OPENCV_INCLUDE})

# Solution(in VS) Name
project(${EXE_NAME})

add_executable(${EXE_NAME} SolarSystem.cpp)

target_link_libraries(${EXE_NAME} opengl32 freeglut glu32 opencv_core231d opencv_highgui231d)