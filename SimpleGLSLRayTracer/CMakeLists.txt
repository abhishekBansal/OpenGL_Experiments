cmake_minimum_required(VERSION 2.6)
SET(EXE_NAME SimpleGLSLRayTracer)

# Specify these before creating target
link_directories($ENV{GLUT_LIB} 
				$ENV{OPENCV_LIB}
				$ENV{GLUI_LIB}) 

include_directories($ENV{GLUT_INCLUDE} 
					$ENV{OPENCV_INCLUDE}
					$ENV{GLUI_INCLUDE}
					$ENV{GLEW_INCLUDE})

# Solution(in VS) Name
project(${EXE_NAME})

add_executable(${EXE_NAME} SimpleGLSLRayTracer.cpp abShaders.cpp abShaders.h) 

target_link_libraries(${EXE_NAME} opengl32 glut32 glu32 glew32 opencv_core231d opencv_highgui231d opencv_imgproc231d glui32d)