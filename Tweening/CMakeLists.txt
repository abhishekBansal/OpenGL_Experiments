cmake_minimum_required(VERSION 2.6)
SET(EXE_NAME Tweening)

# Specify these before creating target
link_directories($ENV{GLUT_LIB} ) 

include_directories($ENV{GLUT_INCLUDE})

# Solution(in VS) Name
project(${EXE_NAME})

add_executable(${EXE_NAME} Tweening.cpp)

target_link_libraries(${EXE_NAME} opengl32 freeglut glu32)