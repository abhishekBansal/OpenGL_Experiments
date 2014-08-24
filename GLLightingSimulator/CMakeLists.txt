cmake_minimum_required(VERSION 2.6)
SET(EXE_NAME GLLightingSimulator)

# Specify these before creating target
link_directories($ENV{GLUT_LIB} 
				 $ENV{GLUI_LIB}
				 $ENV{GLEW_LIB}) 

include_directories($ENV{GLUT_INCLUDE} 
					$ENV{GLUI_INCLUDE}
					$ENV{GLEW_INCLUDE})

# Solution(in VS) Name
project(${EXE_NAME})

add_executable(${EXE_NAME} GLLightingSimulator.cpp) 

target_link_libraries(${EXE_NAME} opengl32 glut32 glu32 glew32 glui32d)